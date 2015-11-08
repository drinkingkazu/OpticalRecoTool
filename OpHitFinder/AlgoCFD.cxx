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
    
    for(const auto& cross : crossings) {

      if( wf.at( cross.first ) > sigma_v.at( cross.first ) * threshold +  mean_v.at( cross.first ) ) {
	
	//we are inside a true pulse, look backward until we go back to baseline...
	_pulse.reset_param();

	int i = cross.first;

	//go backward from CDF 
	while ( wf.at(i) > sigma_v.at(i) * threshold + mean_v.at(i) ) {

	  i--;

	  //watch the edge
	  if ( i < 0 ) {
	    i = 0;
	    break;
	  }
	  
	}
	_pulse.t_start = i; //taking one extra sample as end of pulse
    	  
	//go forward from start point
	while ( wf.at(i) > sigma_v.at(i) * threshold + mean_v.at(i) ) {

	  _pulse.area += wf.at(i);
	  i++;

	  //watch for the edge again
	  if ( i > wf.size() - 1 ) {
	    i = wf.size() - 1;
	    _pulse.area += wf.at(i);
	    break;
	  }
	  
	}
	
	
	_pulse.t_end = i; //taking one extra sample as end of pulse again
	auto it = std::max_element(std::begin(wf) + _pulse.t_start, std::begin(wf) + _pulse.t_end);

	_pulse.t_max    = it - std::begin(wf);
	_pulse.peak     = *it;
	_pulse.ped_mean = 2048; // keep it constant for now until we change struct.
	
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
