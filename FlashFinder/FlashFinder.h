/**
 * \file FlashFinder.h
 *
 * \ingroup SimpleFlashFinder
 * 
 * \brief Class def header for a class FlashFinder
 *
 * @author vgenty
 */

/** \addtogroup FlashFinder

    @{*/

#ifndef LARLITE_FLASHFINDER_H
#define LARLITE_FLASHFINDER_H

#include "Analysis/ana_base.h"

#include "FhiclLite/ConfigManager.h"
#include "FlashAlgoBase.h"
#include "FlashFinderManager.h"

namespace larlite {

  class FlashFinder : public ana_base {
  
  public:

    /// Default constructor
    FlashFinder();

    /// Default destructor
    virtual ~FlashFinder(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);
    
    virtual bool finalize();

    void Configure(const std::string cfg_file)
    { _config_file = cfg_file; }

    unsigned CreateFlashes(const event_ophit* ophits) const;

  private:
    std::string _config_file;
    
    ::fcllite::ConfigManager     _cfg_mgr;
    ::pmtana::FlashFinderManager _mgr;

    std::string _flash_producer;
    std::string _hit_producer;

  };
}
#endif
