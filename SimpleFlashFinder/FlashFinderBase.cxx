#ifndef FLASHFINDERBASE_CXX
#define FLASHFINDERBASE_CXX

#include "FlashFinderBase.h"

namespace pmtana{

  const ::larlite::opflash& FlashFinderBase::GetPulse(size_t index) const
  {

    if(index >= _flash_v.size()) {
      
      std::cerr << "\033[93m"
		<< "Invalid flash index: " << index
		<< "\033[00m"
		<< std::endl;

      throw std::exception();
    }

    else return _flash_v.at(index);

  }
  const std::vector<::larlite::opflash>& FlashFinderBase::GetFlashes() const {
    return _flash_v;
  }
  
}
#endif

