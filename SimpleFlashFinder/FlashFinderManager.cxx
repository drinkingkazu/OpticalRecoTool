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

  
  void FlashFinderManager::SetFlashAlgo (FlashRecoAlgoBase* algo)
  
  {
    if(!algo) {
      std::cerr << "\n\t Not a valid algo\n";
      throw std::exception();
    }
    _flash_algo = algo;
  }

  
  unsigned FlashFinderManager::CreateFlashes(const std::vector<::larlite::ophit>& ophits) const
    
  {
    if(!_flash_algo)
      if(!algo) {
	std::cerr << "\n\t No flashing to run!\n";
	throw std::exception();
      }
    
    auto flashes_produced = _flash_algo->Flash(ophits);
    
    return flashes_produced;
    
  }

}

#endif
