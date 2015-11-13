//really bad flash framework
//by vic

#ifndef FLASHFINDERMANAGER_H
#define FLASHFINDERMANAGER_H

#include "FlashFinderBase.h"

namespace pmtana
{

  class FlashFinderManager {

  public:

    FlashFinderManager();
    
    ~FlashFinderManager();

    void SetFlashAlgo (FlashFinderBase* algo);

    unsigned CreateFlashes(const larlite::event_ophit* ophits) const;
    
  private:
    
    FlashFinderBase* _flash_algo;

  };
  
}
#endif

