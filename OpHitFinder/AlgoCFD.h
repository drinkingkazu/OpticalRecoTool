/**
 * \file AlgoCFD.hh
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of AlgoCFD
 *
 * @author vic - Nevis 2015
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef ALGOCFD_H
#define ALGOCFD_H

#include "PMTPulseRecoBase.h"
//#include "fhiclcpp/ParameterSet.h"
#include "FhiclLite/PSet.h"

namespace pmtana
{

  /**
   \class AlgoCFD
   This class implements threshold algorithm to AlgoCFD class.
  */
  class AlgoCFD : public PMTPulseRecoBase {

  public:

    /// Default constructor
    AlgoCFD(const std::string name="SlidingWindow");

    /// Alternative ctor
    //AlgoCFD(const fhicl::ParameterSet &pset,const std::string name="SlidingWindow");
    AlgoCFD(const ::fcllite::PSet &pset,const std::string name="CFD");

    /// Default destructor
    virtual ~AlgoCFD();
    
    /// Implementation of AlgoCFD::reset() method
    void Reset();

  protected:

    /// Implementation of AlgoCFD::reco() method
    bool RecoPulse(const pmtana::Waveform_t&,
		   const pmtana::PedestalMean_t&,
		   const pmtana::PedestalSigma_t&);
    
    // /// A variable holder for a user-defined absolute ADC threshold value
    // float _adc_thres, _end_adc_thres;

    // /// A variable holder for a multiplicative factor for the pedestal standard deviation to define the threshold.
    // float _nsigma, _end_nsigma;
    // bool _verbose;
    // size_t _num_presample;
  };

}
#endif

/** @} */ // end of doxygen group

