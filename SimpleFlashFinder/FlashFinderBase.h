//by vic

#ifndef FLASHFINDERBASE_H
#define FLASHFINDERBASE_H

// STL

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

    const std::vector<::larlite::opflash>& GetFlashes() const;

  private:

    std::vector<::larlite::opflash> _flash_v;
    
  protected:

    virtual unsigned Flash(const std::vector<::larlite::ophit>& ophits) = 0;

  };

}
#endif

/** @} */ // end of doxygen group
