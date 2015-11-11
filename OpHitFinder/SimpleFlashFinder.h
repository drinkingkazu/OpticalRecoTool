/**
 * \file SimpleFlashFinder.h
 *
 * \ingroup OpHitFinder
 * 
 * \brief Class def header for a class SimpleFlashFinder
 *
 * @author ber9
 */

/** \addtogroup OpHitFinder

    @{*/

#ifndef LARLITE_SIMPLEFLASHFINDER_H
#define LARLITE_SIMPLEFLASHFINDER_H

#include "Analysis/ana_base.h"
#include "FhiclLite/ConfigManager.h"


namespace larlite {
  /**
     \class SimpleFlashFinder
     User custom analysis class made by SHELL_USER_NAME
   */
  class SimpleFlashFinder : public ana_base{
  
  public:

    /// Default constructor
    SimpleFlashFinder(const std::string name="SimpleFlashFinder");

    /// Default destructor
    virtual ~SimpleFlashFinder(){}

    /** IMPLEMENT in SimpleFlashFinder.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in SimpleFlashFinder.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in SimpleFlashFinder.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void Configure(const std::string cfg_file)
    { _config_file = cfg_file; }

  protected:
    
    // config file
    ::fcllite::ConfigManager _cfg_mgr;
    std::string _config_file;
    
  private:

    // declare member data here
    std::string _producer;
    bool _verbose;

    // minimum PE to make a flash
    double _PE_min_flash;

    // minimum PE to use an OpHit
    double _PE_min_hit;

    // bin-width in time
    double _bin_width;
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
