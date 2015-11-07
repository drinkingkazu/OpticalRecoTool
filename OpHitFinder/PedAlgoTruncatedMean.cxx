////////////////////////////////////////////////////////////////////////
//
//  PedAlgoTruncatedMean source
//
////////////////////////////////////////////////////////////////////////

#ifndef larana_OPTICALDETECTOR_PEDALGOTRUNCATEDMEAN_CXX
#define larana_OPTICALDETECTOR_PEDALGOTRUNCATEDMEAN_CXX

#include "PedAlgoTruncatedMean.h"
#include "UtilFunc.h"

namespace pmtana{

  //*****************************************************************
  PedAlgoTruncatedMean::PedAlgoTruncatedMean(const std::string name)
    : PMTPedestalBase(name)
  //*****************************************************************
  {}

  //**************************************************************************
  //PedAlgoTruncatedMean::PedAlgoTruncatedMean(const fhicl::ParameterSet &pset,
  PedAlgoTruncatedMean::PedAlgoTruncatedMean(const ::fcllite::PSet &pset,
					     const std::string name)
    : PMTPedestalBase(name)
  //############################################################
  {

    _sample_size = pset.get<size_t>("SampleSize");

    _max_sigma = pset.get<float>("MaxSigma");

    _ped_range_max = pset.get<float>("PedRangeMax");

    _ped_range_min = pset.get<float>("PedRangeMin");

  }

  //*******************************************
  PedAlgoTruncatedMean::~PedAlgoTruncatedMean()
  //*******************************************
  {}

  //****************************************************************************
  bool PedAlgoTruncatedMean::ComputePedestal( const pmtana::Waveform_t& wf,
					      pmtana::PedestalMean_t&   mean_v,
					      pmtana::PedestalSigma_t&  sigma_v)
  //****************************************************************************
  {

    // parameters
    if(wf.size()<=(_sample_size * 2))
      return false;

    const size_t window_size = _sample_size*2;

    // middle mean
    for(size_t i=0; i< wf.size(); ++i) {

      mean_v[i]  = 0;
      sigma_v[i] = 0;

      if( i < _sample_size || i >= (wf.size() - _sample_size) ) continue;

      mean_v[i]  = mean (wf,i - _sample_size,window_size);
      sigma_v[i] = std  (wf,mean_v[i],i - _sample_size,window_size);
    }

    // front mean
    for(size_t i=0; i<_sample_size; ++i) {

      mean_v[i]  = mean_v[_sample_size];
      sigma_v[i] = sigma_v[_sample_size];
      
    }
    // tail mean
    for(size_t i=(wf.size() - _sample_size); i<wf.size(); ++i) {

      mean_v[i]  = mean_v[wf.size() - _sample_size -1];
      sigma_v[i] = sigma_v[wf.size() - _sample_size -1];

    }
    
    float  best_sigma = 1.1e9;
    size_t best_sigma_index = 0;
    size_t num_good_adc = 0;
    for(size_t i=0; i<sigma_v.size(); ++i) {
      // Only consider adcs which mean is in the allowed range
      auto const& mean  = mean_v[i];
      if( mean < _ped_range_min || mean > _ped_range_max ) continue;

      auto const& sigma = sigma_v[i];
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

    // If not enough # of good mean indices, use the best guess within this waveform
    if(best_sigma > _max_sigma || num_good_adc<3) {
      for(size_t i=0; i<mean_v.size(); ++i) {
	mean_v[i]  = mean_v.at  ( best_sigma_index );
	sigma_v[i] = sigma_v.at ( best_sigma_index );
      }
      //std::cout<<"Only "<<num_good_sigma<<" with min = "<<min_sigma<<std::endl;
      return true;
    }

    // Else do extrapolation
    int last_good_index=-1;
    //size_t good_ctr = 0;
    for(size_t i=0; i<wf.size(); ++i) {
      
      auto const mean  = mean_v[i];
      auto const sigma = sigma_v[i];

      if(sigma <= _max_sigma && mean < _ped_range_max && mean > _ped_range_min) {

	if(last_good_index<0) {
	  last_good_index = (int)i;
	  continue;
	}

	if( (last_good_index+1) < (int)i ) {

	  float slope = (mean - mean_v.at(last_good_index)) / (float(i - last_good_index));

	  for(size_t j=last_good_index+1; j<i; ++j) {
	    mean_v.at(j) = slope * (float(j-last_good_index)) + mean_v.at(last_good_index);
	    sigma_v.at(j) = _max_sigma;
	  }
	}
	last_good_index = (int)i;
      }
    }


    // Next do extrapolation to the first and end
    if(sigma_v.front() > _max_sigma) {
      int first_index=-1;
      int second_index=-1;
      for(size_t i=0; i<wf.size(); ++i) {
	if(sigma_v.at(i)<_max_sigma) {
	  if(first_index<0) first_index = (int)i;
	  else if(second_index<0) {
	    second_index = (int)i;
	    break;
	  }
	}
      }
      if(first_index<0 || second_index<0) throw std::exception();

      float slope = (mean_v.at(second_index) - mean_v.at(first_index)) / (float(second_index - first_index));
      for(int i=0; i<first_index; ++i) {
	mean_v.at(i) = mean_v.at(first_index) - slope * (first_index - i);
	sigma_v.at(i) = _max_sigma;
      }
    }
    
    if(sigma_v.back() > _max_sigma) {
      int first_index=-1;
      int second_index=-1;
      for(int i=wf.size()-1; i>=0; --i) {
	if(sigma_v.at(i)<_max_sigma) {
	  if(second_index<0) second_index = (int)i;
	  else if(first_index<0) {
	    first_index = (int)i;
	    break;
	  }
	}
      }
      float slope = (mean_v.at(second_index) - mean_v.at(first_index)) / (float(second_index - first_index));
      for(int i=second_index+1; i<int(wf.size()); ++i) {
	mean_v.at(i) = mean_v.at(second_index) + slope * (i-second_index);
	sigma_v.at(i) = _max_sigma;
      }
    }
    return true;

  }

}

#endif
