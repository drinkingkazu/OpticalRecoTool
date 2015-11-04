/**
 * \file PMTPulseRecoBase.hh
 *
 * \ingroup PulseReco
 * 
 * \brief Class definition file of PMTPulseRecoBase
 *
 * @author Kazu - Nevis 2013
 */

/** \addtogroup PulseReco
    
@{*/

#ifndef PMTPULSERECOBASE_H
#define PMTPULSERECOBASE_H

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

  struct pulse_param{
  
    double peak, area;
    double t_start, t_max, t_end;
  
    pulse_param(){
      reset_param();
    };
  
    void reset_param(){
      area = 0;
      peak = -1;
      t_start = t_max = t_end = -1;
    };
    
  };

  /**
   \class PMTPulseRecoBase
   The base class of pulse reconstruction algorithms. All algorithms should inherit from this calss
   to be executed by a manager class, pulse_reco. Note that this class does not depend on the rest
   of the framework except for the use of constants. In order to reconstruct a pulse, all it requires
   is a std::vector<short> type object (i.e. raw waveform), waveform pedestal, and its standard
   deviation. All of these are to be provided by an executer. Reconstructed pulse parameters are
   stored in the pulse_param struct object.

   All methods specified as "virtual" should be implemented by the inherit children class.

   This class provides some basic std::vector calculation algorithms such as integral, derivative,
   max and min algorithms. Inherit children classes are encouraged to use these provided methods
   when possible.
  */
  class PMTPulseRecoBase {
    
  public:

    /// Default constructor with fhicl parameters
    PMTPulseRecoBase();

    /// Default destructor
    virtual ~PMTPulseRecoBase();

    /// A method to be called event-wise to reset parameters
    virtual void Reset();

    /** A core method: this executes the algorithm and stores reconstructed parameters
      in the pulse_param struct object.
    */
    virtual bool RecoPulse(const std::vector<short> & /* wf */){return true;};

    /** A getter for the pulse_param struct object. 
      Reconstruction algorithm may have more than one pulse reconstructed from an input waveform.
      Note you must, accordingly, provide an index key to specify which pulse_param object to be retrieved.
    */
    const pulse_param& GetPulse(size_t index=0) const;

    /// A getter for the number of reconstructed pulses from the input waveform
    size_t GetNPulse() const {return _pulse_v.size();};

    /// A setter for the pedestal mean value
    void SetPedMean(double v) { _ped_mean = v; };

    /// A setter for the pedestal standard deviation
    void SetPedRMS(double v) { _ped_rms = v; };

    /// A getter for the set pedestal mean value
    double PedMean() const {return _ped_mean; };

    /// A getter for the set pedestal standard deviation
    double PedRms() const {return _ped_rms; };

  protected:

    /// A container array of pulse_param struct objects to store (possibly multiple) reconstructed pulse(s).
    std::vector<pulse_param> _pulse_v;

    /// A subject pulse_param object to be filled with the last reconstructed pulse parameters
    pulse_param _pulse;

    /// Pedestal mean value
    double _ped_mean;

    /// Pedestal standard deviation
    double _ped_rms;

  protected:

    /**
     A method to integrate an waveform from index "begin" to the "end". The result is filled in "result" reference.
     If the "end" is default (=0), then "end" is set to the last index of the waveform.
    */
    bool Integral   (const std::vector<short> &wf, double &result, size_t begin=0, size_t end=0) const;

    /**
     A method to compute derivative, which is a simple subtraction of previous ADC sample from each sample.
     The result is stored in the input "diff" reference vector which is int32_t type as a derivative could be negative.
    */
    bool Derivative (const std::vector<short> &wf, std::vector<int32_t> &diff, size_t begin=0, size_t end=0) const;

    /**
     A method to return the maximum value of ADC sample within the index from "begin" to "end".
     If the "end" is default (=0), then "end" is set to the last index of the waveform.
    */
    size_t Max(const std::vector<short> &wf, double &result, size_t begin=0, size_t end=0) const;

    /**
     A method to return the minimum value of ADC sample within the index from "begin" to "end".
     If the "end" is default (=0), then "end" is set to the last index of the waveform.
    */
    size_t Min(const std::vector<short> &wf, double &result, size_t begin=0, size_t end=0) const;

  };

}
#endif

/** @} */ // end of doxygen group
