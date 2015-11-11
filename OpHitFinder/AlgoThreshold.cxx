////////////////////////////////////////////////////////////////////////
//
//  AlgoThreshold source
//
////////////////////////////////////////////////////////////////////////

#ifndef ALGOTHRESHOLD_CXX
#define ALGOTHRESHOLD_CXX

#include "AlgoThreshold.h"

namespace pmtana{

  //***************************************************************************
  AlgoThreshold::AlgoThreshold(const std::string name) : PMTPulseRecoBase(name)
  //***************************************************************************
  {
    _adc_thres = 3;
    _nsigma = 5;
    Reset();
  }

  //************************************************************
  //AlgoThreshold::AlgoThreshold(const fhicl::ParameterSet &pset,
  AlgoThreshold::AlgoThreshold(const ::fcllite::PSet &pset,
			       const std::string name)
    : PMTPulseRecoBase(name)
  //*******************************************************
  {

    _adc_thres = pset.get<double>("ADCThreshold");
  
    _nsigma = pset.get<double>("NSigmaThreshold");

    Reset();

  }

  //***************************************************************
  AlgoThreshold::~AlgoThreshold()
  //***************************************************************
  {}

  //***************************************************************
  void AlgoThreshold::Reset()
  //***************************************************************
  {
    PMTPulseRecoBase::Reset();
  }

  //***************************************************************
  bool AlgoThreshold::RecoPulse(const Waveform_t&wf,
				const PedestalMean_t& mean_v,
				const PedestalSigma_t& sigma_v)
  //***************************************************************
  {
    bool fire = false;
    
    double counter=0;

    double ped_mean = mean_v.front();
    double ped_rms  = sigma_v.front();

    double threshold = ( _adc_thres > (_nsigma * ped_rms) ? _adc_thres : (_nsigma * ped_rms) );

    threshold += ped_mean;

    Reset();

    for(auto const &value : wf){
    

      if( !fire && ((double)value) >= threshold ){

	// Found a new pulse

	fire = true;

	_pulse.ped_mean  = ped_mean;
	_pulse.ped_sigma = ped_rms;
	_pulse.t_start = counter;

      }
    
      if( fire && ((double)value) < threshold ){
      
	// Found the end of a pulse

	fire = false;

	_pulse.t_end = counter - 1;
      
	_pulse_v.push_back(_pulse);

	_pulse.reset_param();

      }


      //std::cout << "\tFire=" << fire << std::endl;

      if(fire){

	// Add this adc count to the integral

	_pulse.area += ((double)value - (double)ped_mean);

	if(_pulse.peak < ((double)value - (double)ped_mean)) {

	  // Found a new maximum
	  
	  _pulse.peak = ((double)value - (double)ped_mean);

	  _pulse.t_max = counter;

	}

      }
    
      counter++;
    }

    if(fire){

      // Take care of a pulse that did not finish within the readout window.
    
      fire = false;
    
      _pulse.t_end = counter - 1;
    
      _pulse_v.push_back(_pulse);
    
      _pulse.reset_param();

    }

    return true;

  }
  
}

#endif
