/**
 * \file AlgoPedestal.h
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of AlgoPedestal
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef ALGOPEDESTAL_H
#define ALGOPEDESTAL_H

// STL
#include <set>
#include <vector>
#include <cmath>
#include <functional>
#include <numeric>
#include <iostream>

// ROOT
#include <TString.h>
#include <TTree.h>

namespace pmtana
{


  /**
   \class AlgoPedestal
   A class that calculates pedestal mean & standard deviation (here and elsewhere called as "RMS").   
  */
  class AlgoPedestal {

  public:

    /// Default constructor
    AlgoPedestal();

    /// Default destructor
    virtual ~AlgoPedestal();

    /// Method to compute a pedestal of the input waveform using "nsample" ADC samples from "start" index.
    void ComputePedestal(const std::vector<short>& wf, size_t start, size_t nsample) const;

    /// Getter of the pedestal mean value
    double Mean() const {return _mean;};

    /// Getter of the pedestal standard deviation
    double Sigma() const {return _sigma;};

  protected:

    /// A variable holder for pedestal mean value
    mutable double _mean;

    /// A variable holder for pedestal standard deviation
    mutable double _sigma;


  };
}
#endif

/** @} */ // end of doxygen group
