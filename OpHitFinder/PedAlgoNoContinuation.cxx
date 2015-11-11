////////////////////////////////////////////////////////////////////////
//
//  PedAlgoNoContinuation source
//
////////////////////////////////////////////////////////////////////////

#ifndef larana_OPTICALDETECTOR_PEDALGONOCONTINUATION_CXX
#define larana_OPTICALDETECTOR_PEDALGONOCONTINUATION_CXX

#include "PedAlgoNoContinuation.h"
#include "UtilFunc.h"

#include <ctime>

namespace pmtana{

  //*****************************************************************
  PedAlgoNoContinuation::PedAlgoNoContinuation(const std::string name)
    : PMTPedestalBase(name)
      //*****************************************************************
  {
    srand(static_cast<unsigned int>(time(0)));
  }

  //**************************************************************************
  //PedAlgoNoContinuation::PedAlgoNoContinuation(const fhicl::ParameterSet &pset,
  PedAlgoNoContinuation::PedAlgoNoContinuation(const ::fcllite::PSet &pset,
					     const std::string name)
    : PMTPedestalBase(name)
      //############################################################
  {

    _sample_size    = pset.get<size_t>("SampleSize");

  }

  //*******************************************
  PedAlgoNoContinuation::~PedAlgoNoContinuation()
  //*******************************************
  {}

  //****************************************************************************
  bool PedAlgoNoContinuation::ComputePedestal( const pmtana::Waveform_t& wf,
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

      mean_v[i]  = mean_v [_sample_size];
      sigma_v[i] = sigma_v[_sample_size];
      
    }
    // tail mean
    for(size_t i=(wf.size() - _sample_size); i<wf.size(); ++i) {

      mean_v[i]  = mean_v [wf.size() - _sample_size -1];
      sigma_v[i] = sigma_v[wf.size() - _sample_size -1];

    }

    return true;

  }

}

#endif
