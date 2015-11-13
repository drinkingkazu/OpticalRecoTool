//by vic

#ifndef FLASHFINDERBASE_H
#define FLASHFINDERBASE_H

#include <iostream>

#include "DataFormat/ophit.h"
#include "DataFormat/opflash.h"

#include <vector>

namespace pmtana
{

  class FlashFinderBase {

  public:

    FlashFinderBase();

    virtual ~FlashFinderBase();

    const ::larlite::opflash& GetFlash(size_t index=0) const;

    const ::larlite::event_opflash& GetFlashes() const;

    unsigned RecoFlash(const ::larlite::event_ophit* ophits);

  private:

  protected:

    ::larlite::event_opflash _flash_v;
    
    virtual unsigned Flash(const ::larlite::event_ophit* ophits) = 0;
    
  };

}
#endif

/** @} */ // end of doxygen group
