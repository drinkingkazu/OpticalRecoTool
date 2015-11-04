////////////////////////////////////////////////////////////////////////
//
//  AlgoSlidingWindow source
//
////////////////////////////////////////////////////////////////////////

#ifndef ALGOSLIDINGWINDOW_CXX
#define ALGOSLIDINGWINDOW_CXX

#include "AlgoSlidingWindow.h"

namespace pmtana{

  //############################
  //AlgoSlidingWindow::AlgoSlidingWindow(const fhicl::ParameterSet &pset)
  AlgoSlidingWindow::AlgoSlidingWindow(const ::fcllite::PSet &pset)
  //############################
  {

    _adc_thres = pset.get<float>("ADCThreshold");

    _end_adc_thres = pset.get<float>("EndADCThreshold");
  
    _nsigma = pset.get<float>("NSigmaThreshold");

    _end_nsigma = pset.get<float>("EndNSigmaThreshold");

    _min_wf_size = pset.get<size_t>("MinWindowSize");
    
    _max_sigma = pset.get<float>("MaxSigma");

    _ped_range_max = pset.get<float>("PedRangeMax");

    _ped_range_min = pset.get<float>("PedRangeMin");

    _verbose = pset.get<bool>("Verbosity");

    _num_presample = pset.get<size_t>("NumPreSample");

    Reset();

  }

  //***************************************************************
  AlgoSlidingWindow::~AlgoSlidingWindow()
  //***************************************************************
  {}

  //***************************************************************
  void AlgoSlidingWindow::Reset()
  //***************************************************************
  {
    PMTPulseRecoBase::Reset();
  }

  //**********************************************************************
  bool AlgoSlidingWindow::ConstructPedestal(const std::vector<short> &wf)
  //**********************************************************************
  {
    // parameters
    if(wf.size()<=_min_wf_size) return false;
    
    //
    // Compute pedestal by itself
    //
    _local_mean.resize(wf.size(),0);
    _local_sigma.resize(wf.size(),100);

    for(size_t i=0; i<wf.size(); ++i) {

      if( (i+_min_wf_size) == wf.size() ){
	float last_mean = _local_mean.at(i-1);
	float last_sigma = _local_sigma.at(i-1);
	for(size_t j=i; j<wf.size(); ++j) {
	  _local_mean.at(j) = last_mean;
	  _local_sigma.at(j) = last_sigma;
	}
	break;
      }

      double mean = 0;
      double sigma = 0;
      for(size_t j=0; j<_min_wf_size; ++j) mean += wf.at(i+j);

      mean /= ((float)_min_wf_size);

      for(size_t j=0; j<_min_wf_size; ++j) sigma += pow( (wf.at(i+j) - mean), 2 );

      sigma = sqrt(sigma/((float)(_min_wf_size)));

      _local_mean.at(i)=mean;
      _local_sigma.at(i)=sigma;
    }

    float  best_sigma = 1.1e9;
    size_t best_sigma_index = 0;
    size_t num_good_adc = 0;
    for(size_t i=0; i<_local_sigma.size(); ++i) {
      // Only consider adcs which mean is in the allowed range
      auto const& mean  = _local_mean[i];
      if( mean < _ped_range_min || mean > _ped_range_max ) continue;

      auto const& sigma = _local_sigma[i];
      if(sigma<best_sigma) {
	best_sigma = sigma;
	best_sigma_index = i;
      }
      if(sigma < _max_sigma) num_good_adc += 1;
    }
    
    if( num_good_adc < 1 ) {
      std::cerr << "\033[93m<<" << __FUNCTION__ << ">>\033[00m Could not find good pedestal at all..." << std::endl;
      return false;
    }

    // If no good mean, use the best guess within this waveform
    if(best_sigma > _max_sigma || num_good_adc<3) {
      for(size_t i=0; i<_local_mean.size(); ++i) {
	_local_mean[i]  = _local_mean.at  ( best_sigma_index );
	_local_sigma[i] = _local_sigma.at ( best_sigma_index );
      }
      //std::cout<<"Only "<<num_good_sigma<<" with min = "<<min_sigma<<std::endl;
      return true;
    }

    // Else do extrapolation
    int last_good_index=-1;
    //size_t good_ctr = 0;
    for(size_t i=0; i<wf.size(); ++i) {
      
      auto const mean  = _local_mean[i];
      auto const sigma = _local_sigma[i];

      if(sigma <= _max_sigma && mean < _ped_range_max && mean > _ped_range_min) {

	if(last_good_index<0) {
	  last_good_index = (int)i;
	  continue;
	}

	if( (last_good_index+1) < (int)i ) {

	  float slope = (mean - _local_mean.at(last_good_index)) / (float(i - last_good_index));

	  for(size_t j=last_good_index+1; j<i; ++j) {
	    _local_mean.at(j) = slope * (float(j-last_good_index)) + _local_mean.at(last_good_index);
	    _local_sigma.at(j) = _max_sigma;
	  }
	}
	last_good_index = (int)i;
      }
    }
    //std::cout<<"Used "<<good_ctr<<std::endl;
    // Next do extrapolation to the first and end
    if(_local_sigma.front() > _max_sigma) {
      int first_index=-1;
      int second_index=-1;
      for(size_t i=0; i<wf.size(); ++i) {
	if(_local_sigma.at(i)<_max_sigma) {
	  if(first_index<0) first_index = (int)i;
	  else if(second_index<0) {
	    second_index = (int)i;
	    break;
	  }
	}
      }
      if(first_index<0 || second_index<0) throw std::exception();

      float slope = (_local_mean.at(second_index) - _local_mean.at(first_index)) / (float(second_index - first_index));
      for(int i=0; i<first_index; ++i) {
	_local_mean.at(i) = _local_mean.at(first_index) - slope * (first_index - i);
	_local_sigma.at(i) = _max_sigma;
      }
    }
    
    if(_local_sigma.back() > _max_sigma) {
      int first_index=-1;
      int second_index=-1;
      for(int i=wf.size()-1; i>=0; --i) {
	if(_local_sigma.at(i)<_max_sigma) {
	  if(second_index<0) second_index = (int)i;
	  else if(first_index<0) {
	    first_index = (int)i;
	    break;
	  }
	}
      }
      float slope = (_local_mean.at(second_index) - _local_mean.at(first_index)) / (float(second_index - first_index));
      for(int i=second_index+1; i<int(wf.size()); ++i) {
	_local_mean.at(i) = _local_mean.at(second_index) + slope * (i-second_index);
	_local_sigma.at(i) = _max_sigma;
      }
    }
    return true;
  }


  //***************************************************************
  bool AlgoSlidingWindow::RecoPulse(const std::vector<short> &wf)
  //***************************************************************
  {

    if(!ConstructPedestal(wf)) return false;

    bool fire = false;

    bool in_tail = false;
    
    double pulse_start_threshold=0;
    double pulse_tail_threshold =0;

    double pulse_start_baseline =0;

    //double threshold = ( _adc_thres > (_nsigma * _ped_rms) ? _adc_thres : (_nsigma * _ped_rms) );

    //threshold += _ped_mean;

    Reset();

    for(size_t i=0; i<wf.size(); ++i) {

      auto const& value = wf[i];

      float start_threshold = _local_mean.at(i);
      
      if(_local_sigma.at(i) * _nsigma < _adc_thres) start_threshold += _adc_thres;
      else start_threshold += _local_sigma.at(i) * _nsigma;

      // End pulse if significantly high peak found (new pulse)
      if( (!fire || in_tail) && (double)value > start_threshold ) {


	// If there's a pulse, end it
	if(in_tail) {
	  _pulse.t_end = i - 1;
	  
	  _pulse_v.push_back(_pulse);
	  
	  _pulse.reset_param();
	  
	  if(_verbose)
	    std::cout << "\033[93mPulse End\033[00m: " 
		      << "baseline: " << _local_mean[i] << " ... " << " ... adc: " << value << " T=" << i << std::endl;
	}

	//
	// Found a new pulse ... try to get a few samples prior to this
	//

	pulse_start_threshold = start_threshold;
	pulse_start_baseline  = _local_mean.at(i);

	pulse_tail_threshold = pulse_start_baseline;
	if(_local_sigma.at(i) * _end_nsigma < _end_adc_thres) pulse_tail_threshold += _end_adc_thres;
	else pulse_tail_threshold += _local_sigma.at(i) * _end_nsigma;

	int last_pulse_end_index = 0;
	if(_pulse_v.size()) last_pulse_end_index = _pulse_v.back().t_end;
	int buffer_num_index = (int)i - last_pulse_end_index;
	if(buffer_num_index > (int)_num_presample) buffer_num_index = _num_presample;

	if(buffer_num_index<0) {
	  std::cerr << "\033[95m[ERROR]\033[00m Logic error! Negative buffer_num_index..." << std::endl;
	  throw std::exception();
	}

	_pulse.t_start   = i - buffer_num_index;
	_pulse.ped_mean  = pulse_start_baseline;
	_pulse.ped_sigma = _local_sigma.at(i);

	for(size_t pre_index=_pulse.t_start; pre_index<i; ++pre_index) {

	  auto const& pre_adc = wf[pre_index];
	  if(pre_adc > pulse_start_baseline) continue;

	  _pulse.area += pre_adc - pulse_start_baseline;

	}

	if(_verbose) 
	  std::cout << "\033[93mPulse Start\033[00m: " 
		    << "baseline: " << _local_mean[i] 
		    << " ... threshold: " << start_threshold 
		    << " ... adc: " << value 
		    << " T=" << i << std::endl;

	fire = true;
	in_tail = false;
      }

      if( fire && ((double)value) < pulse_start_threshold ) {
	fire = false;
	in_tail = true;
      }
    
      if( (fire || in_tail) && ((double)value) < pulse_tail_threshold ){
      
	// Found the end of a pulse
	_pulse.t_end = i - 1;
      
	_pulse_v.push_back(_pulse);

	_pulse.reset_param();

	if(_verbose)
	  std::cout << "\033[93mPulse End\033[00m: " 
		    << "baseline: " << _local_mean[i] << " ... " << " ... adc: " << value << " T=" << i << std::endl;
	
	fire = false;
	in_tail = false;

      }

      if(fire || in_tail){

	// Add this adc count to the integral
	double adc_above_baseline = ((double)value - (double)pulse_start_baseline);

	//_pulse.area += ((double)value - (double)_local_mean.at(i));
	_pulse.area += adc_above_baseline;

	if(_pulse.peak < adc_above_baseline) {

	  // Found a new maximum
	  _pulse.peak = adc_above_baseline;

	  _pulse.t_max = i;

	}

      }

    }

    if(fire || in_tail){

      // Take care of a pulse that did not finish within the readout window.
    
      fire = false;
      in_tail = false;

      _pulse.t_end = wf.size() - 1;
    
      _pulse_v.push_back(_pulse);
    
      _pulse.reset_param();

    }

    return true;

  }

}

#endif
