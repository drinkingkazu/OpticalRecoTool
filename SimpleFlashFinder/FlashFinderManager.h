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

    unsigned CreateFlashes(const std::vector<::larlite::ophit>& ophits);
    
    void SetFlashAlgo (pmtana::FlashFinderManager* algo);


  private:

    
    FlashFinderBase* _flash_algo;

  };
  
}
#endif

