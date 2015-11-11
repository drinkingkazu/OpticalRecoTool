/**
 * \file OpHitFinder.h
 *
 * \ingroup OpHitFinder
 * 
 * \brief Class def header for a class OpHitFinder
 *
 * @author kazuhiro
 */

/** \addtogroup OpHitFinder

    @{*/

#ifndef LARLITE_OPHITFINDER_H
#define LARLITE_OPHITFINDER_H

#include "Analysis/ana_base.h"
#include "FhiclLite/ConfigManager.h"
#include "PulseRecoManager.h"
#include "PMTPulseRecoBase.h"

#include "TTree.h"


namespace larlite {
  /**
     \class OpHitFinder
     User custom analysis class made by SHELL_USER_NAME
   */
  class OpHitFinder : public ana_base{
  
  public:

    /// Default constructor
    OpHitFinder(const std::string name="OpHitFinder");

    /// Default destructor
    virtual ~OpHitFinder(){}

    /** IMPLEMENT in OpHitFinder.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in OpHitFinder.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in OpHitFinder.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void Configure(const std::string cfg_file)
    { _config_file = cfg_file; }
    
    bool Reconstruct(const std::vector<short>& wf);

    const std::vector<pmtana::pulse_param>& Pulses() const;

    const ::pmtana::PedestalMean_t PedestalMean() const;

    const ::pmtana::PedestalSigma_t PedestalSigma() const;

    const std::vector<double> CFTrace(const std::vector<short>& wf,
				      const float    F,
				      const unsigned D,
				      const double ped) const;

    //const double LinearZeroPointX(const std::vector<double>& trace) const;
    const std::map<unsigned,double> LinearZeroPointXX(const std::vector<double>& trace);
  protected:
    
    // config file
    ::fcllite::ConfigManager _cfg_mgr;
    std::string _config_file;
    
    // Declare member data here.
    ::pmtana::PulseRecoManager  _preco_mgr;
    std::string _producer, _trigger_producer;
    bool   _use_area;
    double _spe_size;
    bool _verbose;
    ::pmtana::PMTPulseRecoBase* _preco_alg;
    ::pmtana::PMTPedestalBase*  _ped_alg;
  private:

    TTree* _outtree ;

    unsigned _channel;
    double _relTime;
    double _absTime;
    int _frame;
    double _width;
    double _pulse_area;
    double _pulse_peak;
    double _PE;
    double _zero;

    int _is_beamgate;
  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
