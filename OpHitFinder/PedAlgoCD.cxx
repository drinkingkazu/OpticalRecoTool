////////////////////////////////////////////////////////////////////////
//
//  PedAlgoCD source
//
////////////////////////////////////////////////////////////////////////

#ifndef larana_OPTICALDETECTOR_PEDALGOCD_CXX
#define larana_OPTICALDETECTOR_PEDALGOCD_CXX

#include "PedAlgoCD.h"
#include "UtilFunc.h"
#include "OpticalRecoException.h"

namespace pmtana{

  //************************************************
  PedAlgoCD::PedAlgoCD(const std::string name)
    : PMTPedestalBase(name),
      _beamgatealgo(nullptr)
  //************************************************
  {}
  
  //*************************************************************
  //PedAlgoCD::PedAlgoCD(const fhicl::ParameterSet &pset,
  PedAlgoCD::PedAlgoCD(const ::fcllite::PSet &pset,
		       const std::string name,
		       PMTPedestalBase* BeamGateAlgo)
    : PMTPedestalBase(name),
      _beamgatealgo(BeamGateAlgo) //kazu can chamge me
      //*************************************************************
  {
    _beam_gate_samples = pset.get<int>("BeamGateSamples");
  }
  
  //***************************
  PedAlgoCD::~PedAlgoCD()
  //***************************
  {}

  //*********************************************************************
  bool PedAlgoCD::ComputePedestal( const pmtana::Waveform_t& wf,
				   pmtana::PedestalMean_t&   mean_v,
				   pmtana::PedestalSigma_t&  sigma_v)
  //*********************************************************************
  {

    if ( wf.size() < _beam_gate_samples ) { 
    
      double ped_mean  = wf.front(); //first sample
      double ped_sigma = 0;

      for( auto &v : mean_v  ) v = ped_mean;
      for( auto &v : sigma_v ) v = ped_sigma;
    
      return true;
    
    }

    else {
      //How can I call rolling mean without copy paste shit here? 
      _beamgatealgo->Evaluate(wf);
      mean_v  = _beamgatealgo->Mean();
      sigma_v = _beamgatealgo->Sigma();
    }
    
  }

}

#endif
