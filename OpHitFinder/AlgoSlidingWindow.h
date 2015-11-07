/**
 * \file AlgoSlidingWindow.hh
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of AlgoSlidingWindow
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef ALGOSLIDINGWINDOW_H
#define ALGOSLIDINGWINDOW_H

#include "PMTPulseRecoBase.h"
//#include "fhiclcpp/ParameterSet.h"
#include "FhiclLite/PSet.h"

namespace pmtana
{

  /**
   \class AlgoSlidingWindow
   This class implements threshold algorithm to AlgoSlidingWindow class.
  */
  class AlgoSlidingWindow : public PMTPulseRecoBase {

  public:

    /// Default constructor
    AlgoSlidingWindow(const std::string name="SlidingWindow");

    /// Alternative ctor
    //AlgoSlidingWindow(const fhicl::ParameterSet &pset,const std::string name="SlidingWindow");
    AlgoSlidingWindow(const ::fcllite::PSet &pset,const std::string name="SlidingWindow");

    /// Default destructor
    virtual ~AlgoSlidingWindow();
    
    /// Implementation of AlgoSlidingWindow::reset() method
    void Reset();

  protected:

    /// Implementation of AlgoSlidingWindow::reco() method
    bool RecoPulse(const pmtana::Waveform_t&,
		   const pmtana::PedestalMean_t&,
		   const pmtana::PedestalSigma_t&);
    
    /// A variable holder for a user-defined absolute ADC threshold value
    float _adc_thres, _end_adc_thres;

    /// A variable holder for a multiplicative factor for the pedestal standard deviation to define the threshold.
    float _nsigma, _end_nsigma;
    bool _verbose;
    size_t _num_presample;
  };

}
#endif

/** @} */ // end of doxygen group

