#ifndef LARLITE_CHEATFLASHFINDER_CXX
#define LARLITE_CHEATFLASHFINDER_CXX

#include "CheatFlashFinder.h"
#include "DataFormat/opflash.h"
#include "DataFormat/opdetwaveform.h"
#include "DataFormat/simphotons.h"
#include "DataFormat/ophit.h"
#include "DataFormat/mctruth.h"
#include "DataFormat/trigger.h"
#include "LArUtil/Geometry.h"
#include "LArUtil/TimeService.h"

namespace larlite {

  bool CheatFlashFinder::initialize() {

    return true;
  }
  
  bool CheatFlashFinder::analyze(storage_manager* storage) {

    auto const& event_mct   = storage->get_data<event_mctruth>("generator");
    auto const& event_oph   = storage->get_data<event_ophit>(_ophit_producer);
    auto const& event_trig  = storage->get_data<trigger>("triggersim");

    if(!event_mct   || event_mct->empty()   ) { std::cout << "No mctruth found"   << std::endl; return false; }
    if(!event_trig) { std::cout << "No trigger found"   << std::endl; return false; }

    // Flash data product is fucking annoyingly only-set-all-in-constructor style by stupid coder Brian Rabel
    std::vector<double> flash_time_v;
    
    for(auto const& mct : *event_mct) {

      for(auto const& p : mct.GetParticles()) 
	flash_time_v.push_back(p.Trajectory().front().T());

    }

    if(flash_time_v.empty()) return true;

    auto geo = larutil::Geometry::GetME();
    std::vector<std::vector<bool> > hitflag_v(flash_time_v.size(),std::vector<bool>(geo->NOpDets(),true));

    if(_emulate_fem) {

      auto const& event_simph = storage->get_data<event_simphotons>("largeant");
      if(!event_simph || event_simph->empty() ) { std::cout << "No simphoton found" << std::endl; return false; }
      
      // opdet=>opchannel mapping
      std::vector<size_t> opdet2opch(geo->NOpDets(),0);
      for(size_t opch=0; opch<opdet2opch.size(); ++opch)
	
	opdet2opch[geo->OpDetFromOpChannel(opch)] = opch;
      
      std::vector<int> nhit_v(geo->NOpDets(),0);
      
      for(size_t mcf_idx = 0; mcf_idx < flash_time_v.size(); ++mcf_idx) {
	
	auto const& mct = flash_time_v[mcf_idx];
	
	for(auto& nhit : nhit_v) nhit = 0;
	
	for(size_t opdet=0; opdet<geo->NOpDets(); ++opdet) {
	  
	  auto const& simph = (*event_simph)[opdet];
	  
	  for(auto const& oneph : simph) {
	    
	    if( mct > (oneph.Time+100) ) {
	      //std::cout<<"Ignoring photon @ " << oneph.Time << " from interaction @ " << mct << std::endl;
	      continue;
	    }

	    if( oneph.Time - mct < 120 ) nhit_v[opdet2opch[opdet]] += 1;
	  }
	}
	
	for(size_t opch=0; opch<geo->NOpDets(); opch++)

	  hitflag_v[mcf_idx][opch] = (nhit_v[opch] > 4.5);
	
      }
    }

    auto const trig_time = event_trig->TriggerTime();
    auto const ts = larutil::TimeService::GetME();
	
    // Make cheater flash
    auto event_cflash = storage->get_data<event_opflash>(_opflash_producer);
    event_cflash->clear();
    if(event_oph && event_oph->size()) {

      for(size_t mcf_idx=0; mcf_idx<flash_time_v.size(); ++mcf_idx){
	auto const& mc_g4time = flash_time_v[mcf_idx];
	double mc_electime = ts->G4ToElecTime(mc_g4time);
	//std::cout<<"MC time: " << mc_electime << std::endl;
	std::vector<double> pe_v(geo->NOpDets(),0);
	for(auto const& oph : *event_oph) {
	  if(oph.OpChannel() < _ignore_ch_below) continue;
	  if(oph.OpChannel() > _ignore_ch_above) continue;
	  const size_t opch = oph.OpChannel() % 100;
	  //if(oph.PE()>5) std::cout << oph.OpChannel() << " ... " << oph.PE() << " @ " << oph.PeakTime() << " or " << oph.PeakTimeAbs() << std::endl;
	  if(oph.PeakTimeAbs() < mc_electime || oph.PeakTimeAbs() > (mc_electime + 8)) {
	    /*
	    std::cout << "Ignoring channel " << oph.OpChannel() 
		      << " pulse @ " << oph.PeakTimeAbs() 
		      << " trigger = " << trig_time << std::endl;
	    */
	    continue;
	  }
	  if(_emulate_fem && oph.PeakTimeAbs() > (mc_electime +0.625))
	    continue;
	  if(hitflag_v[mcf_idx][opch])
	    pe_v[opch] += oph.PE();
	}
	for(auto& pe : pe_v) pe -= (_emulate_fem ? 0.625/8. * 2.0 : 2.0);
	
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

  bool CheatFlashFinder::finalize() {

    // This function is called at the end of event loop.
    // Do all variable finalization you wish to do here.
    // If you need, you can store your ROOT class instance in the output
    // file. You have an access to the output file through "_fout" pointer.
    //
    // Say you made a histogram pointer h1 to store. You can do this:
    //
    // if(_fout) { _fout->cd(); h1->Write(); }
    //
    // else 
    //   print(MSG::ERROR,__FUNCTION__,"Did not find an output file pointer!!! File not opened?");
    //
  
    return true;
  }

}
#endif
