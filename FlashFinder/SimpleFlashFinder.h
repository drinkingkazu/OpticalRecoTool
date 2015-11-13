//By David C. Ported to larlite by Brooke. ruined by vic in new framework.

#ifndef SIMPLEFLASHFINDER_H
#define SIMPLEFLASHFINDER_H

#include "FlashFinderBase.h"
#include "FhiclLite/ConfigManager.h"

namespace pmtana
{

  class SimpleFlashFinder : public FlashFinderBase {

  public:

    SimpleFlashFinder();

    SimpleFlashFinder(const ::fcllite::PSet &p);
    
    virtual ~SimpleFlashFinder();

  private:

    double TotalCharge(const std::vector<double>& PEs);

    // minimum PE to make a flash
    double _PE_min_flash;

    // minimum PE to use an OpHit
    double _PE_min_hit;

    // bin-width in time
    double _bin_width;
      
  protected:

    unsigned Flash(const ::larlite::event_ophit* ophits);
    
  };

}
#endif

/** @} */ // end of doxygen group
