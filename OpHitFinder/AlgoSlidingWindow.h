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
    //AlgoSlidingWindow(const fhicl::ParameterSet &pset);
    AlgoSlidingWindow(const ::fcllite::PSet &pset);

    /// Default destructor
    virtual ~AlgoSlidingWindow();

    /// Implementation of AlgoSlidingWindow::reco() method
    virtual bool RecoPulse(const std::vector<short> &wf);
    
    /// Implementation of AlgoSlidingWindow::reset() method
    virtual void Reset();

    bool ConstructPedestal(const std::vector<short> &wf);

    /// A method to set user-defined ADC threshold value
    void SetADCSlidingWindow(double v) {_adc_thres = v;};

    /** 
      A method to set a multiplication factor to the pedestal standard deviation
      which is used as one of two input values to define a threshold.
    */
    void SetNSigma(double v) {_nsigma = v;};

    void SetMinWindowSize(size_t v) {_min_wf_size = v;}

    void SetMaxSigma(float v) {_max_sigma = v;}
  
  protected:

    /// A variable holder for a user-defined absolute ADC threshold value
    float _adc_thres, _end_adc_thres;

    /// A variable holder for a multiplicative factor for the pedestal standard deviation to define the threshold.
    float _nsigma, _end_nsigma;
    bool _verbose;
    size_t _min_wf_size, _num_presample;
    float  _max_sigma;
    float  _ped_range_max;
    float  _ped_range_min;
    std::vector<float> _local_mean;
    std::vector<float> _local_sigma;
  
  };

}
#endif

/** @} */ // end of doxygen group
