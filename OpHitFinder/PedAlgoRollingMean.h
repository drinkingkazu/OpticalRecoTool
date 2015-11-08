/**
 * \file PedAlgoRollingMean.h
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of PedAlgoRollingMean
 *
 * @author vic - Nevis 2015
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef larana_OPTICALDETECTOR_PEDALGOROLLINGMEAN_H
#define larana_OPTICALDETECTOR_PEDALGOROLLINGMEAN_H

// STL
#include "PMTPedestalBase.h"
#include "FhiclLite/PSet.h"

namespace pmtana
{

  /**
   \class PedAlgoRollingMean
   A class that calculates pedestal mean & standard deviation (here and elsewhere called as "RMS").   
  */
  class PedAlgoRollingMean : public PMTPedestalBase{

  public:

    /// Default constructor
    PedAlgoRollingMean(const std::string name="PedRollingMean");

    /// Alternative ctor
    //PedAlgoRollingMean(const fhicl::ParameterSet &pset,const std::string name="PedTruncatedMean");
    PedAlgoRollingMean(const ::fcllite::PSet &pset,const std::string name="PedRollingMean");

    /// Default destructor
    virtual ~PedAlgoRollingMean();
    
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

    
    const double get_max(const std::vector<double>& v ,int bins) const;

  };
}
#endif

/** @} */ // end of doxygen group
