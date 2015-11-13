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
#include "FlashFinderBase.h"
#include "FlashFinderManager.h"

namespace larlite {

  class FlashFinder : public ana_base {
  
  public:

    /// Default constructor
    FlashFinder(){ _name="FlashFinder"; _fout=0;}

    /// Default destructor
    virtual ~FlashFinder(){}

    virtual bool initialize();

    virtual bool analyze(storage_manager* storage);
    
    virtual bool finalize();

    void Configure(const std::string cfg_file)
    { _config_file = cfg_file; }

    unsigned CreateFlashes(const std::vector<::larlite::ophit>& ophits);
    
    
  protected:

  private:
    std::string _config_file;
    
    ::fcllite::ConfigManager     _cfg_mgr;
    ::pmtana::FlashFinderManager* _preco_mgr;
    
  };
}
#endif
