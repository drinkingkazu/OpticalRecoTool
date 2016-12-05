#ifndef LARLITE_FLASHCOMP_CXX
#define LARLITE_FLASHCOMP_CXX

#include "FlashComp.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/opflash.h"
#include "LArUtil/TimeService.h"
namespace larlite {

  bool FlashComp::initialize() {

    _flash_tree = new TTree(Form("%s_flashtree",_tree_prefix.c_str()),"");
    _flash_tree->Branch("bnb",&_bnb,"bnb/I");
    _flash_tree->Branch("mcflash_pe",&_mcflash_pe,"mcflash_pe/D");
    _flash_tree->Branch("cheat_pe",&_cheat_pe,"cheat_pe/D");
    _flash_tree->Branch("opflash_pe",&_opflash_pe,"opflash_pe/D");
    _flash_tree->Branch("simple_pe",&_simple_pe,"simple_pe/D");
    _flash_tree->Branch("mcflash_t",&_mcflash_t,"mcflash_t/D");
    _flash_tree->Branch("cheat_dt",&_cheat_dt,"cheat_dt/D");
    _flash_tree->Branch("opflash_dt",&_opflash_dt,"opflash_dt/D");
    _flash_tree->Branch("simple_dt",&_simple_dt,"simple_dt/D");

    _event_tree = new TTree(Form("%s_eventtree",_tree_prefix.c_str()),"");
    _event_tree->Branch("mcflash_ctr",&_mcflash_ctr,"mcflash_ctr/I");
    _event_tree->Branch("cheat_ctr",&_cheat_ctr,"cheat_ctr/I");
    _event_tree->Branch("opflash_ctr",&_opflash_ctr,"opflash_ctr/I");
    _event_tree->Branch("simple_ctr",&_simple_ctr,"simple_ctr/I");
    
    return true;
  }
  
  bool FlashComp::analyze(storage_manager* storage) {

    auto ev_mctruth = storage->get_data<event_mctruth>("generator");
    auto ev_mcflash = storage->get_data<event_opflash>("mcflash");
    auto ev_simple  = storage->get_data<event_opflash>(_simple_producer);
    auto ev_opflash = storage->get_data<event_opflash>(_opflash_producer);
    auto ev_cheat   = storage->get_data<event_opflash>(_cheat_producer);

    // Get BNB timing
    double bnb_time=-1;
    auto ts = larutil::TimeService::GetME();
    for(auto const& mct : *ev_mctruth) {
      if(mct.Origin() == 1) {
	bnb_time = ts->G4ToElecTime(bnb_time);
	break;
      }
    }

    std::vector<std::pair<double,double> > mcflash_info_v;
    mcflash_info_v.reserve(ev_mcflash->size());
    // Loop over MCFlash
    for(auto const& opf : *ev_mcflash) {
      double totpe = 0;
      for(size_t i=0; i<32; ++i) totpe += opf.PE(i);
      if(totpe <10) continue;
      mcflash_info_v.emplace_back(std::move(std::pair<double,double>(opf.Time(),totpe)));
    }

    std::vector<std::pair<double,double> > simple_info_v;
    simple_info_v.reserve(ev_simple->size());
    // Loop over Simple
    for(auto const& opf : *ev_simple) {
      double totpe = 0;
      for(size_t i=0; i<32; ++i) totpe += opf.PE(i);
      if(totpe <10) continue;
      simple_info_v.emplace_back(std::move(std::pair<double,double>(opf.Time(),totpe)));
    }
    
    std::vector<std::pair<double,double> > opflash_info_v;
    if(ev_opflash) {
      opflash_info_v.reserve(ev_opflash->size());
      // Loop over Opflash
      for(auto const& opf : *ev_opflash) {
	double totpe = 0;
	for(size_t i=0; i<32; ++i) totpe += opf.PE(i);
	if(totpe <10) continue;
	opflash_info_v.emplace_back(std::move(std::pair<double,double>(opf.Time(),totpe)));
      }
    }
    
    std::vector<std::pair<double,double> > cheat_info_v;
    cheat_info_v.reserve(ev_cheat->size());
    // Loop over Cheat
    for(auto const& opf : *ev_cheat) {
      double totpe = 0;
      for(size_t i=0; i<32; ++i) totpe += opf.PE(i);
      if(totpe <10) continue;
      cheat_info_v.emplace_back(std::move(std::pair<double,double>(opf.Time(),totpe)));    
    }
    
    // Now let's go fill
    for(auto const& mcflash_info : mcflash_info_v) {

      _bnb = 0;
      _mcflash_pe=mcflash_info.second;
      _mcflash_t=mcflash_info.first;
      _cheat_pe=0;
      _opflash_pe=0;
      _simple_pe=0;

      _cheat_dt=-1;
      _opflash_dt=-1;
      _simple_dt=-1;

      if(bnb_time > 0) {
	double dt = mcflash_info.first - bnb_time;
	if(-0.2 < dt < 0.2) _bnb=1;
      }
      
      for(auto const& simple_info : simple_info_v) {
	if(simple_info.first < (mcflash_info.first-0.5)) continue;
	if(simple_info.first > (mcflash_info.first+2)) continue;
	_simple_pe = simple_info.second;
	_simple_dt = simple_info.first - mcflash_info.first;
	break;
      }

      for(auto const& opflash_info : opflash_info_v) {
	if(opflash_info.first < (mcflash_info.first-0.5)) continue;
	if(opflash_info.first > (mcflash_info.first+2)) continue;
	_opflash_pe = opflash_info.second;
	_opflash_dt = opflash_info.first - mcflash_info.first;
	break;
      }

      for(auto const& cheat_info : cheat_info_v) {
	if(cheat_info.first < (mcflash_info.first-0.5)) continue;
	if(cheat_info.first > (mcflash_info.first+2)) continue;
	_cheat_pe = cheat_info.second;
	_cheat_dt = cheat_info.first - mcflash_info.first;
	break;
      }

      _flash_tree->Fill();
    }

    _simple_ctr = simple_info_v.size();
    _cheat_ctr  = cheat_info_v.size();
    _mcflash_ctr = mcflash_info_v.size();
    _opflash_ctr = opflash_info_v.size();

    _event_tree->Fill();
    
    return true;
  }

  bool FlashComp::finalize() {

    if(_fout) {
      _fout->cd();
      _flash_tree->Write();
      _event_tree->Write();
    }

    return true;
  }

}
#endif
