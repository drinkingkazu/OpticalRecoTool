#ifndef LARLITE_FLASHFINDER_CXX
#define LARLITE_FLASHFINDER_CXX

#include "FlashFinder.h"
#include "LArUtil/TimeService.h"
#include "DataFormat/ophit.h"
#include "DataFormat/opflash.h"

namespace larlite {
  
  FlashFinder::FlashFinder(){
    
    _name="FlashFinder";
    
    _fout=0;
  }
  
  bool FlashFinder::initialize() {
    
    _cfg_mgr.AddCfgFile(_config_file);
    
    auto const& main_cfg = _cfg_mgr.Config();
    
    auto const p = main_cfg.get_pset(_name);
    
    _hit_producer   = p.get<std::string>("OpHitProducer");
    _flash_producer = p.get<std::string>("OpFlashProducer");

    auto const flash_algo = p.get<std::string>("FlashFinderAlgo");
    auto const flash_pset = main_cfg.get_pset(flash_algo);
    auto algo_ptr = ::pmtana::FlashAlgoFactory::get().create(flash_algo,flash_algo);
    algo_ptr->Configure(flash_pset);
    _mgr.SetFlashAlgo(algo_ptr);
    
    return true;
  }
      
  bool FlashFinder::analyze(storage_manager* storage) {

    event_opflash* opflashes = storage->get_data<event_opflash>(_flash_producer);
    if(!opflashes) {
      std::cerr<<"\033[93mFailed to create an output w/ name: \033[00m"<<_flash_producer.c_str()<<std::endl;
      throw std::exception();
    }
    opflashes->clear();
    storage->set_id(storage->run_id(),storage->subrun_id(),storage->event_id());

    auto const ophitHandle = storage->get_data<event_ophit>(_hit_producer);
    if(!ophitHandle) {
      std::cerr<<"\033[93mInvalid Producer name: \033[00m"<<_hit_producer.c_str()<<std::endl;
      throw std::exception();
    }
    if(ophitHandle->empty()) {
      std::cout<<"No ophit found by " << _hit_producer << " in event " <<ophitHandle->event_id()<<std::endl;
      return true;
    }

    ::pmtana::LiteOpHitArray_t ophits;
    double trigger_time=1.1e20;
    for(auto const& oph : *ophitHandle) {
      ::pmtana::LiteOpHit_t loph;
      if(trigger_time > 1.e20) trigger_time = oph.PeakTimeAbs() - oph.PeakTime();
      loph.peak_time = oph.PeakTime();
      loph.pe = oph.PE();
      loph.channel = oph.OpChannel();
      ophits.emplace_back(std::move(loph));
    }

    auto const flash_v = _mgr.RecoFlash(ophits);

    auto const ts = ::larutil::TimeService::GetME();
    for(const auto& lflash :  flash_v) {
      //std::cout<<lflash.time<<std::endl;
      larlite::opflash flash(lflash.time, lflash.time_err,
			     trigger_time + lflash.time,
			     ts->OpticalClock().Frame(trigger_time + lflash.time),
			     lflash.channel_pe);
      opflashes->emplace_back(std::move(flash));
    }
    
    return true;
  }
  
  bool FlashFinder::finalize() {
    
    return true;
  }

}
#endif
