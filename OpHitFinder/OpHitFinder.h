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
    
    const std::vector<pmtana::pulse_param>& Reconstruct(const std::vector<short>& wf);
    const std::pair< std::vector<double>,
		     std::vector<double> > ReconstructBaseline(const std::vector<short>& wf,
							       const int ws);
	
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

  private:

    template<typename T>
    std::vector<std::vector<T> > windows(const std::vector<T>& the_thing,
					 const int window_size) const;
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
