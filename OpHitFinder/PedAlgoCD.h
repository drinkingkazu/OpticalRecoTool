/**
 * \file PedAlgoCD.h
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of PedAlgoCD
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef larana_OPTICALDETECTOR_PEDALGOCD_H
#define larana_OPTICALDETECTOR_PEDALGOCD_H

// STL
#include "PMTPedestalBase.h"
#include "FhiclLite/PSet.h"

namespace pmtana
{

  /**
   \class PedAlgoCD
   A class that calculates pedestal mean & standard deviation (here and elsewhere called as "RMS").   
  */
  class PedAlgoCD : public PMTPedestalBase{

  public:

    /// Default constructor
    PedAlgoCD(const std::string name="PedCD");
    
    ///Alternative ctor
    //PedAlgoCD(const fhicl::ParameterSet &pset,const std::string name="PedEdges");
    //PedAlgoCD(const ::fcllite::PSet &pset,const std::string name="PedCD");
    PedAlgoCD(const ::fcllite::PSet &pset,
	      const std::string name,
	      PMTPedestalBase* BeamGateAlgo);

    /// Default destructor
    virtual ~PedAlgoCD();

  protected:

    /// Method to compute a pedestal of the input waveform using "nsample" ADC samples from "start" index.
    bool ComputePedestal( const pmtana::Waveform_t& wf,
			  pmtana::PedestalMean_t&   mean_v,
			  pmtana::PedestalSigma_t&  sigma_v);
    
  private:

    PMTPedestalBase * _beamgatealgo;
    int _beam_gate_samples;
  };
}
#endif

/** @} */ // end of doxygen group
