////////////////////////////////////////////////////////////////////////
//
//  FlasFinderManager source
//
////////////////////////////////////////////////////////////////////////

#ifndef FLASHFINDERMANAGER_CXX
#define FLASHFINDERMANAGER_CXX

#include "FlashFinderManager.h"
#include <sstream>

namespace pmtana{
  
  FlashFinderManager::FlashFinderManager() :
    _flash_algo(nullptr)  
  {}

  FlashFinderManager::~FlashFinderManager() {}

  
  void FlashFinderManager::SetFlashAlgo (FlashFinderBase* algo)
  
  {
    if(!_flash_algo) {
      std::cerr << "\n\t Not a valid algo\n";
      throw std::exception();
    }
    _flash_algo = algo;
  }

  unsigned FlashFinderManager::CreateFlashes(const larlite::event_ophit* ophits) const
  {
    if(!_flash_algo) {
      std::cerr << "\n\t No flashing to run!\n";
      throw std::exception();
    }
    
    auto flashes_produced = _flash_algo->RecoFlash(ophits);
    
    return flashes_produced;
    
  }

}

#endif
