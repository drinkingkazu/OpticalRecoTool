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
    AlgoCFD(const std::string name="CFD");

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
    

    const std::map<unsigned,double> LinearZeroPointX(const std::vector<double>& trace);
    
  private:
    float _F;
    int   _D;
    
    //int    _number_presample; 
    double _peak_thresh;
    double _start_thresh;
    double _end_thresh;    
    
    
  };

}
#endif

/** @} */ // end of doxygen group

