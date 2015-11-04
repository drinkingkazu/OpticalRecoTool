/**
 * \file PulseRecoManager.h
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of PulseRecoManager
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef PULSERECOMANAGER_H
#define PULSERECOMANAGER_H

//#include "fhiclcpp/ParameterSet.h"
//#include "messagefacility/MessageLogger/MessageLogger.h"

#include "PMTPulseRecoBase.h"
#include "AlgoPedestal.h"
namespace pmtana
{

  /// enum to define ped_estimator algorithm options
  enum PED_METHOD{
    kHEAD = 0, ///< Use first N samples
    kTAIL,     ///< Use last N samples
    kBOTH      ///< Calculate both and use the one with smaller RMS
  };

  /**
   \class PulseRecoManager
   A manager class of pulse reconstruction which acts as an analysis unit (inherits from ana_base).
   This class executes various pulse reconstruction algorithm which inherits from PMTPulseRecoBase
   Refer to analyze() function implementation to check how a pulse reconstruction algorithm is called.
  */
  class PulseRecoManager {

  public:

    /// Default constructor
    PulseRecoManager();
    
    /// Default constructor 
    ~PulseRecoManager();

    /// Implementation of ana_base::analyze method
    bool RecoPulse(const std::vector<short>& fifo) const;

    /// A method to set pulse reconstruction algorithm
    void AddRecoAlgo          (PMTPulseRecoBase* algo) { _reco_algo_v.push_back(algo); };
  
    /// A method to set a choice of pedestal estimation method
    void SetPedAlgo           (PED_METHOD type) { _ped_method = type; };

    /// A method to set # of ADC samples to be used for a cosmic readout
    void SePedSampleCosmic (size_t n) { _ped_nsample_cosmic = n; };

    /// A method to set # of ADC samples to be used for a beam readout
    void SetPedSampleBeam   (size_t n) { _ped_nsample_beam   = n; };

  private:

    /// pulse reconstruction algorithm pointer
    std::vector<PMTPulseRecoBase*> _reco_algo_v;

    /// ped_estimator object
    AlgoPedestal _ped_algo;

    /// # samples to be used for pedestal estimation of cosmic readout
    size_t _ped_nsample_cosmic;

    /// # samples to be used for pedestal estimation of beam readout
    size_t _ped_nsample_beam;

    /// Enum value to be set by a user for the pedestal estimation method.
    PED_METHOD _ped_method;

    /// A method to judge whether the subject waveform corresponds to a beam readout or not
    inline bool is_beam(const std::vector<short> &wf) const {return (wf.size() > 1000);};

  };
}
#endif

/** @} */ // end of doxygen group
