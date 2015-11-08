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
//#include "AlgoSlidingWindowTwo.h"
#include "AlgoFixedWindow.h"
#include "PedAlgoEdges.h"
#include "PedAlgoTruncatedMean.h"
#include "PedAlgoRollingMean.h"
#include "OpticalRecoException.h"

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

    //auto const pset = p.get<fhicl::ParameterSet>("HitAlgoPset");
    //auto const pset = p.get<fhicl::ParameterSet>("HitAlgoPset");

    std::string hit_alg_name = p.get<std::string>("HitFinder");

    auto const hit_pset = main_cfg.get_pset(hit_alg_name);

    if(hit_alg_name == "Threshold")
      _preco_alg = new pmtana::AlgoThreshold(hit_pset,hit_alg_name);
    else if(hit_alg_name == "FixedWindow")
      _preco_alg = new pmtana::AlgoFixedWindow(hit_pset,hit_alg_name);
    else if(hit_alg_name == "SlidingWindow")
      _preco_alg = new pmtana::AlgoSlidingWindow(hit_pset,hit_alg_name);
    //else if(hit_alg_name == "SlidingWindowTwo")
    //_preco_alg = new pmtana::AlgoSlidingWindowTwo(hit_pset,hit_alg_name);
    else {
      std::stringstream ss;
      ss << "Invalid PulseReco algorithm name: " << hit_alg_name;
      throw ::pmtana::OpticalRecoException(ss.str());
    }

    std::string ped_alg_name = p.get<std::string>("Pedestal");

    auto const ped_pset = main_cfg.get_pset(ped_alg_name);

    if(ped_alg_name == "PedEdges")
      _ped_alg = new pmtana::PedAlgoEdges(ped_pset,ped_alg_name);
    else if(ped_alg_name == "PedTruncatedMean")
      _ped_alg = new pmtana::PedAlgoTruncatedMean(ped_pset,ped_alg_name);
    else if(ped_alg_name == "PedRollingMean") {
      _ped_alg = new pmtana::PedAlgoRollingMean(ped_pset,ped_alg_name);
      std::cout << ped_alg_name << std::endl;
    }
    
    else {
      std::stringstream ss;
      ss << "Invalid Pedestal algorithm name: " << ped_alg_name;
      throw ::pmtana::OpticalRecoException(ss.str());
    }    
    _preco_mgr.SetDefaultPedAlgo(_ped_alg);
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
      _preco_mgr.Reconstruct(wf_ptr);

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

  bool OpHitFinder::Reconstruct(const std::vector<short>& wf)
  {

    if(!_preco_alg) {

      std::cerr << "\033[93m[ERROR]\033[00m "
		<< "Pulse reco algorithm not yet set. Make sure to call initialize() before anything!"
		<< std::endl;

      throw std::exception();
    }

    return _preco_mgr.Reconstruct(wf);

  }

  const std::vector<pmtana::pulse_param>& OpHitFinder::Pulses() const
  { return _preco_alg->GetPulses(); }
  
  const ::pmtana::PedestalMean_t OpHitFinder::PedestalMean() const
  { return _ped_alg->Mean(); }
  
  const ::pmtana::PedestalSigma_t OpHitFinder::PedestalSigma() const
  { return _ped_alg->Sigma(); }
  
  const std::vector<double> OpHitFinder::CFTrace(const std::vector<short>& wf,
						 const float F,
						 const unsigned D,
						 const double ped) const {

    std::vector<double> cfd; cfd.reserve(wf.size());

    /// we are going to lose D values of of each end since we go left to right
    /// later we can go the other direction
    for (unsigned k = 0; k < wf.size(); ++k)  {

      auto delayed = F *  ( (float) wf.at(k) - ped);

      if (k < D)

	cfd.push_back( -1.0 * delayed);

      else

	cfd.push_back( -1.0 * delayed +  ( (float) wf.at(k - D) - ped ) );
    }
    

    return cfd;
  }
  
  //const double OpHitFinder::LinearZeroPointX(const std::vector<double>& trace) const {
  const std::map<unsigned,double> OpHitFinder::LinearZeroPointX(const std::vector<double>& trace) const {

     auto x = double{0.0};
     std::map<unsigned,double> crossing;
     //step through the trace and find where slope is POSITIVE across zero

     for ( unsigned i = 0; i < trace.size() - 1; ++i) {

       auto si = sign(trace.at(i));
       auto sf = sign(trace.at(i+1));

       if ( si == sf ) //no sign flip, no zero cross
	 continue;
       
       if ( sf < si ) //this is a negative slope, continue
	 continue;

       //calculate the crossing X based on linear interpolation bt two pts

       crossing[i] = (double) i - trace.at(i) * ( 1.0 / ( trace.at(i+1) - trace.at(i) ) );
       
     }
     
     // for (const auto& m : crossing )
     //   std::cout << "index: "  << m.first << " X intercept: " << m.second << "\n";
     return crossing;
     //return x;
  }
  
  template<typename W>
  int OpHitFinder::sign(W val) const{
    
    if (val > 0) return  1;
    if (val < 0) return -1;
    return 0;
    
  }

  bool OpHitFinder::finalize() {

    return true;
  }

}
#endif
