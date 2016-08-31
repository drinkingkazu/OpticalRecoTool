#ifndef LARLITE_MCFLASHFINDER_CXX
#define LARLITE_MCFLASHFINDER_CXX

#include "MCFlashFinder.h"
#include "DataFormat/opflash.h"
#include "DataFormat/opdetwaveform.h"
#include "DataFormat/ophit.h"
#include "DataFormat/simphotons.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/trigger.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/TimeService.h"
namespace larlite {

  bool MCFlashFinder::initialize() {

    return true;
  }
  
  bool MCFlashFinder::analyze(storage_manager* storage) {
  
    auto const& event_mct   = storage->get_data<event_mctruth>("generator");
    auto const& event_simph = storage->get_data<event_simphotons>("largeant");
    auto const& event_oph   = storage->get_data<event_ophit>("ophitSat");
    auto const& event_opwf  = storage->get_data<event_opdetwaveform>("saturation");
    auto const& event_trig  = storage->get_data<trigger>("triggersim");

    if(!event_mct   || event_mct->empty()   ) { std::cout << "No mctruth found"   << std::endl; return false; }
    if(!event_simph || event_simph->empty() ) { std::cout << "No simphoton found" << std::endl; return false; }
    if(!event_trig) { std::cout << "No trigger found"   << std::endl; return false; }

    auto event_mcflash = storage->get_data<event_opflash>("mcflash");
    if(!event_mcflash) { std::cout << "mcflash could not be created..." << std::endl; return false; }

    // Flash data product is fucking annoyingly only-set-all-in-constructor style by stupid coder Brian Rabel
    std::vector<double> flash_time_v;
    
    for(auto const& mct : *event_mct)

      flash_time_v.push_back(mct.GetParticles().front().Trajectory().front().T());

    if(flash_time_v.empty()) return true;
    

    auto geo = larutil::Geometry::GetME();
    std::vector<std::vector<double> > pmt_v(flash_time_v.size(),std::vector<double>(geo->NOpDets(),0));

    if(event_simph->size() != geo->NOpDets()) {
      std::cout << "Unexpected # of channels in simphotons!" << std::endl;
      throw std::exception();
    }

    // opdet=>opchannel mapping
    std::vector<size_t> opdet2opch(geo->NOpDets(),0);
    for(size_t opch=0; opch<opdet2opch.size(); ++opch)

      opdet2opch[geo->OpDetFromOpChannel(opch)] = opch;

    for(size_t mcf_idx = 0; mcf_idx < flash_time_v.size(); ++mcf_idx) {

      auto const& mct = flash_time_v[mcf_idx];
      auto& pe_v = pmt_v[mcf_idx];
      
      for(size_t opdet=0; opdet<pe_v.size(); ++opdet) {

	auto const& simph = (*event_simph)[opdet];

	for(auto const& oneph : simph) {

	  if( mct > (oneph.Time+100) ) {
	    std::cout<<"Ignoring photon @ " << oneph.Time << " from interaction @ " << mct << std::endl;
	    continue;
	  }

	  if( oneph.Time - mct < 100000 ) pe_v[opdet2opch[opdet]] += 1;
	  //if( oneph.Time - mct < 100000 ) pe_v[opdet] += 1;
	  
	}
      }
    }

    auto const trig_time = event_trig->TriggerTime();
    auto const ts = larutil::TimeService::GetME();
	
    for(size_t mcf_idx=0; mcf_idx < flash_time_v.size(); ++mcf_idx) {

      opflash f( ts->G4ToElecTime(flash_time_v[mcf_idx]) - trig_time,       // flash time w.r.t. trigger
		 0,                                       // time width
		 ts->G4ToElecTime(flash_time_v[mcf_idx]), // flash time in elec clock
		 0,                                       // FIXME frame number
		 pmt_v[mcf_idx],                          // PE array
		 false,                                   // FIXME in beam frame boolean
		 0,                                       // FIXME on beam time boolean
		 1,                                       // FIXME fast-to-total pe ratio
		 0,                                       // FIXME y center
		 0,                                       // FIXME y width
		 0,                                       // FIXME z center
		 0                                        // FIXME z width
		 );

      event_mcflash->emplace_back(std::move(f));
    }

    // Make cheater flash
    auto event_cflash = storage->get_data<event_opflash>("cheatFlash");
    if(event_oph && event_opwf && event_oph->size()) {

      // Get beamwindow start t
      double wf_start=0;
      for(auto const& wf : *event_opwf) {
	if(wf.size() < 1000) continue;
	wf_start = wf.TimeStamp();
	break;
      }
      for(auto const& mc_g4time : flash_time_v) {

	double mc_electime = ts->G4ToElecTime(mc_g4time);
	std::vector<double> pe_v(geo->NOpDets(),0);
	for(auto const& oph : *event_oph) {
	  if(oph.OpChannel() > 31) continue;
	  if(oph.PeakTimeAbs() < mc_electime || oph.PeakTimeAbs() > (mc_electime + 8)) {
	    std::cout << "Ignoring pulse @ " << oph.PeakTimeAbs() << " trigger = " << trig_time << std::endl;
	    continue;
	  }
	  pe_v[oph.OpChannel()] += oph.PE();
	}
	
	opflash f( ts->G4ToElecTime(mc_g4time) - trig_time,       // flash time w.r.t. trigger
		   0,                           // time width
		   ts->G4ToElecTime(mc_g4time), // flash time in elec clock
		   0,                           // FIXME frame number
		   pe_v,                        // PE array
		   false,                       // FIXME in beam frame boolean
		   0,                           // FIXME on beam time boolean
		   1,                           // FIXME fast-to-total pe ratio
		   0,                           // FIXME y center
		   0,                           // FIXME y width
		   0,                           // FIXME z center
		   0                            // FIXME z width
		 );
	
	event_cflash->emplace_back(std::move(f));
      }
    }    

    storage->set_id(storage->run_id(),storage->subrun_id(),storage->event_id());
    
    return true;
  }

  bool MCFlashFinder::finalize() {

    return true;
  }

}
#endif
