#ifndef FLASHFINDERBASE_CXX
#define FLASHFINDERBASE_CXX

#include "FlashFinderBase.h"

namespace pmtana{

  FlashFinderBase::FlashFinderBase() { Reset(); }

  FlashFinderBase::~FlashFinderBase() {}

  void FlashFinderBase::Reset() {
    _flash_v.clear();
    _flash_v.reserve(1);
  }
  
  unsigned FlashFinderBase::RecoFlash(const ::larlite::event_ophit* ophits) {
    return this->Flash(ophits);
  }
  
  const ::larlite::opflash& FlashFinderBase::GetFlash(size_t index) const
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
  const ::larlite::event_opflash& FlashFinderBase::GetFlashes() const {
    return _flash_v;
  }
  
}
#endif

