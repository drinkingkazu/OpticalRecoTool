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


  // const std::vector<double> OpHitFinder::CFTrace(const std::vector<short>& wf,
  // 						 const float F,
  // 						 const unsigned D,
  // 						 const double ped) const {
    
    auto F   = 0;
    auto D   = 0;
    auto ped = 0;
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
    
    
    //   return cfd;
    // }
    
    
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
