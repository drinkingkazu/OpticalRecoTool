/**
 * \file PedAlgoNoContinuation.h
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of PedAlgoNoContinuation
 *
 * @author Kazu, Vic - Nevis 2015 
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef larana_OPTICALDETECTOR_PEDALGONOCONTINUATION_H
#define larana_OPTICALDETECTOR_PEDALGONOCONTINUATION_H

// STL
#include "PMTPedestalBase.h"
#include "FhiclLite/PSet.h"

namespace pmtana
{

  /**
   \class PedAlgoNoContinuation
   A class that calculates pedestal mean & standard deviation (here and elsewhere called as "RMS").   
  */
  class PedAlgoNoContinuation : public PMTPedestalBase{

  public:

    /// Default constructor
    PedAlgoNoContinuation(const std::string name="PedNoContinuation");

    /// Alternative ctor
    //PedAlgoNoContinuation(const fhicl::ParameterSet &pset,const std::string name="PedNoContinuation");
    PedAlgoNoContinuation(const ::fcllite::PSet &pset,const std::string name="PedNoContinuation");

    /// Default destructor
    virtual ~PedAlgoNoContinuation();
    
  protected:

    /// Method to compute a pedestal of the input waveform using "nsample" ADC samples from "start" index.
    bool ComputePedestal( const pmtana::Waveform_t& wf,
			  pmtana::PedestalMean_t&   mean_v,
			  pmtana::PedestalSigma_t&  sigma_v);
    
  private:

    size_t _sample_size;

  };
}
#endif

/** @} */ // end of doxygen group
