////////////////////////////////////////////////////////////////////////
//
//  AlgoCFD source
//
////////////////////////////////////////////////////////////////////////

#ifndef ALGOCFD_CXX
#define ALGOCFD_CXX

#include "AlgoCFD.h"

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










    return true;

  }

}

#endif
