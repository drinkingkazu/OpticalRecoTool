#ifndef LARLITE_FLASHFINDER_CXX
#define LARLITE_FLASHFINDER_CXX

#include "FlashFinder.h"

namespace larlite {
  
  FlashFinder::FlashFinder(){
    
    _flash_algo = nullptr;
    
    _fout=0;
  }
  
  bool FlashFinder::initialize() {
    
    _cfg_mgr.AddCfgFile(_config_file);
    
    auto const& main_cfg = _cfg_mgr.Config();
    
    auto const p = main_cfg.get_pset(_name);
    
    _flash_finder_algo = p.get<std::string>("FlashFinderAlgo");
    
    _hit_producer      = p.get<std::string>("OpHitProducer");
    _flash_producer    = p.get<std::string>("OpFlashProducer");
    
    
    auto const flash_pset = main_cfg.get_pset(_flash_finder_algo);
    
    if(_flash_finder_algo == "SimpleFlashFinder")
      _preco_alg = new pmtana::SimpleFlashFinder(flash_pset);
    else if(_flash_finder_algo == "NotSimpleFlashFinder")
      _preco_alg = new pmtana::NotSimpleFlashFinder(flash_pset);
    
    else {
      std::cerr << "Invalid PulseReco algorithm name: " << _flash_finder_algo << "\n";
      throw std::exception();
    }
    
    _preco_mgr.SetFlashAlgo(_preco_alg);
    
    return true;
  }
      
  bool FlashFinder::analyze(storage_manager* storage) {
      
    auto const ophitHandle = storage->get_data<event_ophit>(_hit_producer);

    storage->set_id(storage->run_id(),storage->subrun_id(),storage->event_id());
    
    if(!ophitHandle || ophitHandle->empty()){
      std::cerr<<"\033[93mInvalid Producer name: \033[00m"<<_producer.c_str()<<std::endl;
      throw std::exception();
    }

    event_opflash* opflashes = storage->get_data<event_opflash>(_name);


    
    auto n_flashes = _preco_mgr.CreateFlashes(ophitHandle);

    for(const auto& flash :  _preco_alg->GetFlashes() ) 

      event_opflash->push_back(flash);
    
    
    return true;
  }
  
  unsigned FlashFinder::CreateFlashes(const std::vector<::larlite::ophit>& ophits) const {

    return _preco_mgr.CreateFlashes(ophits);
    
  }
  
  bool FlashFinder::finalize() {

  
    return true;
  }

}
#endif
