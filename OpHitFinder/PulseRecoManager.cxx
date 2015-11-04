////////////////////////////////////////////////////////////////////////
//
//  PulseRecoManager source
//
////////////////////////////////////////////////////////////////////////

#ifndef PULSERECOMANAGER_CC
#define PULSERECOMANAGER_CC

#include "PulseRecoManager.h"

namespace pmtana{

  //##################################
  PulseRecoManager::PulseRecoManager()
  //##################################
  {

    _reco_algo_v.clear();

    _ped_nsample_cosmic = 1;

    _ped_nsample_beam   = 8;

    _ped_method = kHEAD;

  }

  //***************************************************************
  PulseRecoManager::~PulseRecoManager()
  //***************************************************************
  {}

  //***************************************************************
  bool PulseRecoManager::RecoPulse(const std::vector<short> &fifo) const
  //***************************************************************
  {
    bool status = true;

    //
    // Step 0: skipe 0-length waveform with a warning message
    //    
    if(fifo.size()<1){
      
      std::cerr <<"Found 0-length waveform vector!" << std::endl;

      return false;
    }
    
    //
    // Step 1: apply pedestal estimation
    //  
    double ped_mean = 0;
    double sigma  = 0;
    // Figure out whether this is a beam readout or not
    size_t ped_nsample = ( is_beam(fifo) ? _ped_nsample_beam : _ped_nsample_cosmic);
    
    switch(_ped_method){

    case kHEAD:
      
      _ped_algo.ComputePedestal(fifo, 0, ped_nsample);
      
      ped_mean = _ped_algo.Mean();
      
      sigma  = _ped_algo.Sigma();
      
      break;
      
    case kTAIL:
      
      _ped_algo.ComputePedestal(fifo, (fifo.size()-ped_nsample), ped_nsample);
      
      ped_mean = _ped_algo.Mean();
      
      sigma  = _ped_algo.Sigma();
      
      break;
      
    case kBOTH:
      
      _ped_algo.ComputePedestal(fifo, 0, ped_nsample);
      
      ped_mean = _ped_algo.Mean();
      
      sigma  = _ped_algo.Sigma();
      
      _ped_algo.ComputePedestal(fifo, (fifo.size()-ped_nsample), ped_nsample);
      
      if( sigma > _ped_algo.Sigma() ) {
	
	ped_mean = _ped_algo.Mean();
	
	sigma  = _ped_algo.Sigma();
	
      }
      
      break;
    }
    
    //
    // Step 2: apply reco algos
    //
    for(auto reco_algo : _reco_algo_v){
      
      reco_algo->SetPedMean(ped_mean);
      
      reco_algo->SetPedRMS (sigma);
      
      status = status && reco_algo->RecoPulse(fifo);

    } // end of reco algorithm loop
    return status;
  
  }

}

#endif
