/**
 * \file AlgoThreshold.hh
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of AlgoThreshold
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef ALGOTHRESHOLD_H
#define ALGOTHRESHOLD_H

#include "PMTPulseRecoBase.h"
//#include "fhiclcpp/ParameterSet.h"
#include "FhiclLite/PSet.h"

namespace pmtana
{

  /**
   \class AlgoThreshold
   This class implements threshold algorithm to AlgoThreshold class.
   The algorithm defines a pulse in user-specified time window.
   A typical usage is to set the beginning of the window to be 0 (= start of the waveform)
   and integrate over the time of interest. By default, the ending is set to index=0, in 
   which case it uses the ending index of the input waveform (i.e. full integration).
  */
  class AlgoThreshold : public PMTPulseRecoBase {

  public:

    /// Default constructor
    AlgoThreshold();

    /// Alternative constructor
    //AlgoThreshold(const fhicl::ParameterSet &pset);
    AlgoThreshold(const ::fcllite::PSet &pset);

    /// Default destructor
    virtual ~AlgoThreshold();

    /// Implementation of AlgoThreshold::reco() method
    virtual bool RecoPulse(const std::vector<short> &wf);
    
    /// Implementation of AlgoThreshold::reset() method
    virtual void Reset();

    /// A method to set user-defined ADC threshold value
    void SetADCThreshold(double v) {_adc_thres = v;};

    /** 
      A method to set a multiplication factor to the pedestal standard deviation
      which is used as one of two input values to define a threshold.
    */
    void SetNSigma(double v) {_nsigma = v;};
  
  protected:

    /// A variable holder for a user-defined absolute ADC threshold value
    double _adc_thres;

    /// A variable holder for a multiplicative factor for the pedestal standard deviation to define the threshold.
    double _nsigma;
  
  };

}
#endif

/** @} */ // end of doxygen group
