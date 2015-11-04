/**
 * \file AlgoFixedWindow.h
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of AlgoFixedWindow
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef ALGOFIXEDWINDOW_H
#define ALGOFIXEDWINDOW_H

#include "PMTPulseRecoBase.h"
//#include "fhiclcpp/ParameterSet.h"
#include "FhiclLite/PSet.h"

namespace pmtana
{

  /**
   \class AlgoFixedWindow
   This class implements fixed_window algorithm to AlgoFixedWindow class.
   The algorithm defines a pulse in user-specified time window.
   A typical usage is to set the beginning of the window to be 0 (= start of the waveform)
   and integrate over the time of interest. By default, the ending is set to index=0, in 
   which case it uses the ending index of the input waveform (i.e. full integration).
  */
  class AlgoFixedWindow : public PMTPulseRecoBase {

  public:

    /// Default constructor
    AlgoFixedWindow();
    
    /// Alternative ctor
    //AlgoFixedWindow(const fhicl::ParameterSet &pset);
    AlgoFixedWindow(const ::fcllite::PSet &pset);

    /// Default destructor
    virtual ~AlgoFixedWindow();

    /// Implementation of AlgoFixedWindow::reco() method
    virtual bool RecoPulse(const std::vector<short> &wf);
    
    /// Implementation of AlgoFixedWindow::reset() method
    virtual void Reset();

    /// Specify the beginning of fixed pulse time window
    void SetStartSample(size_t index) {_index_start = index;};

    /// Specify the end of fixed pulse time window
    void SetEndSample  (size_t index) {_index_end   = index;};
  
  protected:

    size_t _index_start; ///< index marker for the beginning of the pulse time window
    size_t _index_end;   ///< index marker for the end of pulse time window

  };

}
#endif

/** @} */ // end of doxygen group
