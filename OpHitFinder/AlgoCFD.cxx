////////////////////////////////////////////////////////////////////////
//
//  AlgoCFD source
//
////////////////////////////////////////////////////////////////////////

#ifndef ALGOCFD_CXX
#define ALGOCFD_CXX

#include "AlgoCFD.h"
#include "UtilFunc.h"

namespace pmtana{

  //*********************************************************************
  AlgoCFD::AlgoCFD(const std::string name)
    : PMTPulseRecoBase(name)
  //*********************************************************************
  {}

  //*********************************************************************
  //AlgoCFD::AlgoCFD(const fhicl::ParameterSet &pset,
  AlgoCFD::AlgoCFD(const ::fcllite::PSet &pset,
		   const std::string name)
    : PMTPulseRecoBase(name)
      //*********************************************************************
  {

    _F = pset.get<float>("Fraction");
    _D = pset.get<int>  ("Delay");
    
    
    // _adc_thres = pset.get<float>("ADCThreshold");
    // _end_adc_thres = pset.get<float>("EndADCThreshold");
    // _nsigma = pset.get<float>("NSigmaThreshold");
    // _end_nsigma = pset.get<float>("EndNSigmaThreshold");
    // _verbose = pset.get<bool>("Verbosity");
    // _num_presample = pset.get<size_t>("NumPreSample");

    Reset();

  }

  //***************************************************************
  AlgoCFD::~AlgoCFD()
  //***************************************************************
  {}

  //***************************************************************
  void AlgoCFD::Reset()
  //***************************************************************
  {
    PMTPulseRecoBase::Reset();
  }

  //***************************************************************
  bool AlgoCFD::RecoPulse(const pmtana::Waveform_t& wf,
			  const pmtana::PedestalMean_t& mean_v,
			  const pmtana::PedestalSigma_t& sigma_v)
  //***************************************************************
  {

    Reset();
  // const std::vector<double> OpHitFinder::CFTrace(const std::vector<short>& wf,
  // 						 const float F,
  // 						 const unsigned D,
  // 						 const double ped) const {
    
    auto ped = 0;

    std::vector<double> cfd; cfd.reserve(wf.size());

    /// we are going to lose D values of of each end since we go left to right
    /// later we can go the other direction
    for (unsigned k = 0; k < wf.size(); ++k)  {
      
      auto delayed = -1.0 * _F *  ( (float) wf.at(k) - mean_v.at(k) );

      if (k < _D)
	
	cfd.push_back( delayed );

      else

	cfd.push_back(delayed +  ( (float) wf.at(k - _D) - mean_v.at(k) ) );
    }


    //Get the zero point crossings, how can I tell which are meaningful?
    //go to each crossing, see if waveform is above pedestal (high above pedestal
    auto crossings = LinearZeroPointX(cfd);
    auto threshold = 5.0;
    auto number_presample = 5;
    double before_mean, after_mean;
    before_mean = after_mean = 0;

    auto in_peak = [&wf,&sigma_v,&mean_v,&threshold](int i) -> bool
      { return wf.at(i) > sigma_v.at(i) * threshold + mean_v.at(i); };
    
    for(const auto& cross : crossings) {

      if( wf.at( cross.first ) > sigma_v.at( cross.first ) * threshold +  mean_v.at( cross.first ) ) {
	
	_pulse.reset_param();

	int i = cross.first;

	//backwards
	//while ( wf.at(i) > sigma_v.at(i) * threshold + mean_v.at(i) ) {
	while (in_peak(i) ){
	  i--;
	  if ( i < 0 ) { i = 0; break; }
	}
	_pulse.t_start = i;
	
	//walk a little further backwards to see if we can get 5 low RMS
	//while ( wf.at(i) < sigma_v.at(i) * threshold + mean_v.at(i) ) {
	while ( !in_peak(i) ) {
	  if (i == ( _pulse.t_start - number_presample ) ) break;
	  i--;
	  if ( i < 0 ) { i = 0; break; }
	}
	
	before_mean = std::accumulate(std::begin(mean_v) + i,
				      std::begin(mean_v) + _pulse.t_start, 0.0) / ((double) (_pulse.t_start - i));

	i = _pulse.t_start + 1;
	
	//forwards
	//while ( wf.at(i) > sigma_v.at(i) * threshold + mean_v.at(i) ) {
	while ( in_peak(i) ) {
	  i++;
	  if ( i > wf.size() - 1 ) { i = wf.size() - 1; break; }
	}
		
	_pulse.t_end = i;

	//walk a little further forwards to see if we can get 5 low RMS
	//while ( wf.at(i) < sigma_v.at(i) * threshold + mean_v.at(i) ) {
	while ( !in_peak(i) ) {
	  if (i == ( _pulse.t_end + number_presample ) ) break;
	  i++;
	  if ( i > wf.size() - 1 ) { i = wf.size() - 1; break; }
	}

	after_mean = std::accumulate(std::begin(mean_v) + _pulse.t_end + 1,
				     std::begin(mean_v) + i + 1, 0.0) / ((double) (i - _pulse.t_end));
	

	//how to decide before or after? set before for now
	_pulse.ped_mean = before_mean;

	if(wf.size() < 50) _pulse.ped_mean = mean_v.front(); //is COSMIC DISCRIMINATOR

	auto it = std::max_element(std::begin(wf) + _pulse.t_start, std::begin(wf) + _pulse.t_end);

	_pulse.t_max      =  it - std::begin(wf);
	_pulse.peak       = *it - _pulse.ped_mean;
	_pulse.t_cdfcross =  cross.second;

	for(auto k = _pulse.t_start; k <= _pulse.t_end; ++k)
	  _pulse.area += wf.at(i) - _pulse.ped_mean;
	  
	_pulse_v.push_back(_pulse);
      }
      
    }
    
    
    return true;
    
  }

  // currently returns ALL zero point crossings, we really just want ones associated with peak...
  const std::map<unsigned,double> AlgoCFD::LinearZeroPointX(const std::vector<double>& trace) {

    std::map<unsigned,double> crossing;
    
    //step through the trace and find where slope is POSITIVE across zero
    for ( unsigned i = 0; i < trace.size() - 1; ++i) {

      auto si = ::pmtana::sign(trace.at(i));
      auto sf = ::pmtana::sign(trace.at(i+1));

      if ( si == sf ) //no sign flip, no zero cross
	continue;
       
      if ( sf < si ) //this is a negative slope, continue
	continue;

      //calculate the crossing X based on linear interpolation bt two pts


      crossing[i] = (double) i - trace.at(i) * ( 1.0 / ( trace.at(i+1) - trace.at(i) ) );
      
    }
    

    return crossing;

  }


}

#endif
