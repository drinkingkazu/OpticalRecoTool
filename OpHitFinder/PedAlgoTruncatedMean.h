/**
 * \file PedAlgoTruncatedMean.h
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of PedAlgoTruncatedMean
 *
 * @author Kazu, Vic - Nevis 2015 
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef larana_OPTICALDETECTOR_PEDALGOTRUNCATEDMEAN_H
#define larana_OPTICALDETECTOR_PEDALGOTRUNCATEDMEAN_H

// STL
#include "PMTPedestalBase.h"
#include "FhiclLite/PSet.h"

namespace pmtana
{

  /**
   \class PedAlgoTruncatedMean
   A class that calculates pedestal mean & standard deviation (here and elsewhere called as "RMS").   
  */
  class PedAlgoTruncatedMean : public PMTPedestalBase{

  public:

    /// Default constructor
    PedAlgoTruncatedMean(const std::string name="PedTruncatedMean");

    /// Alternative ctor
    //PedAlgoTruncatedMean(const fhicl::ParameterSet &pset,const std::string name="PedTruncatedMean");
    PedAlgoTruncatedMean(const ::fcllite::PSet &pset,const std::string name="PedTruncatedMean");

    /// Default destructor
    virtual ~PedAlgoTruncatedMean();
    
  protected:

    /// Method to compute a pedestal of the input waveform using "nsample" ADC samples from "start" index.
    bool ComputePedestal( const pmtana::Waveform_t& wf,
			  pmtana::PedestalMean_t&   mean_v,
			  pmtana::PedestalSigma_t&  sigma_v);
    
  private:
    size_t _sample_size;
    float  _max_sigma;
    float  _ped_range_max;
    float  _ped_range_min;

  };
}
#endif

/** @} */ // end of doxygen group
