#ifndef LARLITE_OPHITFINDER_CXX
#define LARLITE_OPHITFINDER_CXX

#include "OpHitFinder.h"
#include "DataFormat/ophit.h"
#include "DataFormat/opdetwaveform.h"
#include "DataFormat/trigger.h"
#include "LArUtil/TimeService.h"
#include "LArUtil/Geometry.h"
#include "AlgoThreshold.h"
#include "AlgoSlidingWindow.h"
#include "AlgoSlidingWindowTwo.h"
#include "AlgoFixedWindow.h"


#include "TH1D.h"

namespace larlite {

  OpHitFinder::OpHitFinder(const std::string name) : _cfg_mgr("OpHitFinder")
  {
    _name=name;
    _config_file = "";
    _preco_alg = nullptr;
  }
  
  bool OpHitFinder::initialize() {

    //std::cout<<_config_file<<std::endl;
    
    _cfg_mgr.AddCfgFile(_config_file);

    auto const& main_cfg = _cfg_mgr.Config();

    //std::cout << main_cfg.dump() << std::endl;
    
    // Call appropriate produces<>() functions here.
    //produces< std::vector<recob::OpHit>   >();
    auto const p = main_cfg.get_pset(_name);
    _producer = p.get<std::string>("OpDetWaveformProducer");
    _trigger_producer = p.get<std::string>("TriggerProducer");
    _use_area = p.get<bool>("UseArea");
    _spe_size = p.get<double>("SPESize");

    _verbose  = p.get<bool>("Verbosity");

    _preco_mgr.SetPedAlgo(pmtana::kHEAD);
    _preco_mgr.SePedSampleCosmic (  3 );
    _preco_mgr.SetPedSampleBeam  ( 10 );

    //auto const pset = p.get<fhicl::ParameterSet>("HitAlgoPset");
    //auto const pset = p.get<fhicl::ParameterSet>("HitAlgoPset");

    std::string hit_alg_name = p.get<std::string>("HitFinder");

    auto const pset = main_cfg.get_pset(hit_alg_name);
    
    if(hit_alg_name == "Threshold")
      _preco_alg = new pmtana::AlgoThreshold(pset);
    else if(hit_alg_name == "FixedWindow")
      _preco_alg = new pmtana::AlgoFixedWindow(pset);
    else if(hit_alg_name == "SlidingWindow")
      _preco_alg = new pmtana::AlgoSlidingWindow(pset);
    else if(hit_alg_name == "SlidingWindowTwo")
      _preco_alg = new pmtana::AlgoSlidingWindowTwo(pset);

    _preco_mgr.AddRecoAlgo(_preco_alg);
    
    return true;
  }
  
  bool OpHitFinder::analyze(storage_manager* storage) {

    auto const wfHandle = storage->get_data<event_opdetwaveform>(_producer);
    auto const trigHandle = storage->get_data<trigger>(_trigger_producer);
    
    if (!wfHandle || wfHandle->empty()) {
      std::cerr<<"\033[93mInvalid Producer name: \033[00m"<<_producer.c_str()<<std::endl;
      throw std::exception();
    }

    if (!trigHandle) {
      std::cerr<<"\033[93mInvalid Producer name: \033[00m"<<_trigger_producer.c_str()<<std::endl;      
      throw std::exception();
    }

    auto const trigger_time = trigHandle->TriggerTime();
    auto const beam_time    = trigHandle->BeamGateTime();

    event_ophit* ophits = storage->get_data<event_ophit>(_name);
    
    //art::ServiceHandle<geo::Geometry> geom;
    //art::ServiceHandle<util::TimeService> ts;
    auto const geom = ::larutil::Geometry::GetME();
    auto const ts   = ::larutil::TimeService::GetME();

    for(auto const& wf_ptr : *wfHandle) {

      const int Channel = (int)wf_ptr.ChannelNumber();
      const double TimeStamp = wf_ptr.TimeStamp();

      if(_verbose) std::cout << "Processing channel: " << Channel << std::endl;

      if(Channel>32) continue;

      if( !geom->IsValidOpChannel( Channel ) ) {
	std::cerr << "Error! unrecognized channel number " << Channel<<". Ignoring pulse";
	continue;
      }


      /// Reconstruct the pulse
      _preco_mgr.RecoPulse(wf_ptr);

      /// Get the result
      auto const& pulses = _preco_alg->GetPulses();

      if(pulses.empty())
	
	std::cout << "\033[95m[WARNING]\033[00m PulseFinder algorithm returned invalid status! (Ch. " << Channel << ")" << std::endl;

      for(size_t k=0; k<pulses.size(); ++k){

	auto const& pulse = pulses[k];

	double AbsTime = TimeStamp + pulse.t_max * ts->OpticalClock().TickPeriod();

	double RelTime = AbsTime - beam_time;
	if(beam_time<0)
	  RelTime = AbsTime - trigger_time;

	int    Frame   = ts->OpticalClock().Frame(TimeStamp);

	double PE      = 0.0;
	if (_use_area) PE = pulse.area / _spe_size;
	else           PE = pulse.peak / _spe_size;

	double width   = ( pulse.t_end - pulse.t_start ) * ts->OpticalClock().TickPeriod();

	ophits->emplace_back( Channel, RelTime, AbsTime, Frame, width, pulse.area, pulse.peak, PE, 0. );
      }
    }
    //e.put(std::move(ophits));
  
    return true;
  }

  const std::vector<pmtana::pulse_param>& OpHitFinder::Reconstruct(const std::vector<short>& wf)
  {

    
    if(!_preco_alg) {

      std::cerr << "\033[93m[ERROR]\033[00m "
		<< "Pulse reco algorithm not yet set. Make sure to call initialize() before anything!"
		<< std::endl;

      throw std::exception();
    }

    _preco_mgr.RecoPulse(wf);

    return _preco_alg->GetPulses();

  }
  
  
  const std::vector<double> OpHitFinder::CFTrace(const std::vector<short>& wf,
						 const float F,
						 const unsigned D) const {

    std::vector<double> cfd; cfd.reserve(wf.size());

    /// we are going to lose D values of of each end since we go left to right
    /// later we can go the other direction
    for (unsigned k = 0; k < wf.size(); ++k)  {
      auto delayed = F *  ( (float) wf.at(k) );
      if (k < D)
	cfd.push_back( -1.0 * delayed);
      else
	cfd.push_back(-1.0 * delayed +  ( (float) wf.at(k - D) ) );
    }
    
    //temporary, extend vector to match length of usual one
    // while(cfd.size() < wf.size())

    //   cfd.push_back(cfd.back());
    
    return cfd;
  }
  
  // const double OpHitFinder::LinearZeroPointX(const std::vector<W>& trace) const {

  //   auto x = double{0.0};
    
    
    
  //   return x;
  // }
  
  // const std::pair< std::vector<double>,
  // 		   std::vector<double> > OpHitFinder::ReconstructBaseline(const std::vector<short>& wf,
  // 									  const int ws)
  
  const std::pair<double,double> OpHitFinder::ReconstructBaseline(const std::vector<short>& wf,
								  const int ws)
  {

    
    if(!_preco_alg) {

      std::cerr << "\033[93m[ERROR]\033[00m "
		<< "Pulse reco algorithm not yet set. Make sure to call initialize() before anything!"
		<< std::endl;

      throw std::exception();
    }

    //Calculate pedestal baseline and return it
    auto ped_mean  = double{0.0};
    auto ped_sigma = double{0.0};

    std::vector<double> local_mean;
    std::vector<double> local_sigma;

    local_mean.reserve(wf.size());
    local_sigma.reserve(wf.size());

    for(const auto& window : windows(wf,ws) ) {

      ped_mean  = 0.0;
      ped_sigma = 0.0;
      
      double nsample = window.size();
      
      for(const auto & w : window)

	ped_mean += w;

      ped_mean /= nsample;
            
      for(const auto & w : window)
	
	ped_sigma += pow( w - ped_mean , 2 );
      
      ped_sigma = sqrt ( ped_sigma / nsample );

      
      local_mean.push_back (ped_mean);
      local_sigma.push_back(ped_sigma);
      
    }

    // I don't know if I can write a faster histogram algo than in ROOT
    // so for now I steal TH1D...
    int bins = 25; // this determines I guess pedestal resolution

    auto max_mean  = get_max(local_mean,  bins);
    auto max_sigma = get_max(local_sigma, bins);
    
    std::cout << " max of mean : " << max_mean  << "\n";
    std::cout << " max of sigma: " << max_sigma << "\n";
    
    //return std::make_pair(local_mean,local_sigma);
    return std::make_pair(max_mean,max_sigma);
  }

  
  const double OpHitFinder::get_max(const std::vector<double>& v ,int bins) const {
    
    auto max_it = std::max_element(std::begin(v), std::end(v));
    auto min_it = std::min_element(std::begin(v), std::end(v));

    TH1D th("th",";;",bins,*max_it,*min_it);
    
    for (const auto & m : v) th.Fill(m);

    return th.GetXaxis()->GetBinCenter(th.GetMaximumBin());

  }
  
  bool OpHitFinder::finalize() {

    return true;
  }



  template<typename T>
  std::vector<std::vector<T> > OpHitFinder::windows(const std::vector<T>& thing,
						    const int window_size) const
  {
    
    std::vector<std::vector<T> > data;
    
    auto w = window_size + 2;
    w = (unsigned int)((w - 1)/2);
    auto num = thing.size();

    if(window_size > num) {
      std::cerr << "\033[93m<<" << __FUNCTION__ << ">>\033[00m window_size > num" << std::endl;
      throw std::exception();
    }
    
    data.reserve(num);
    
    for(int i = 1; i <= num; ++i) {
      std::vector<T> inner;
      inner.reserve(20);
      if(i < w) {
	for(int j = 0; j < 2 * (i%w) - 1; ++j)
	  inner.push_back(thing[j]);
      }else if (i > num - w + 1){
	for(int j = num - 2*((num - i)%w)-1 ; j < num; ++j)
	  inner.push_back(thing[j]);
      }else{
	for(int j = i - w; j < i + w - 1; ++j)
	  inner.push_back(thing[j]);
      }
      data.emplace_back(inner);
    }

    return data;
  
  }

}
#endif
