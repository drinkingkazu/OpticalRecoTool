#ifndef LARLITE_SIMPLEFLASHFINDER_CXX
#define LARLITE_SIMPLEFLASHFINDER_CXX

#include "SimpleFlashFinder.h"

// #include "DataFormat/ophit.h"
#include "DataFormat/opflash.h"

namespace larlite {

  SimpleFlashFinder::SimpleFlashFinder(const std::string name) : _cfg_mgr("SimpleFlashFinder")
  {
    _name=name;
    _config_file = "";
  }
  
  bool SimpleFlashFinder::initialize()
  {

    _cfg_mgr.AddCfgFile(_config_file);

    auto const& main_cfg = _cfg_mgr.Config();
    
    // call appropriate produces<>() functions
    auto const p = main_cfg.get_pset(_name);
    _producer            = p.get<std::string>("OpFlashProducer");
    _min_time_integrated = p.get<double>("MinTimeIntegrated");
  
    _PE_min_flash = 10; //50;
    _PE_min_hit = 1;
    _bin_width = 0.1; // usec

    _verbose = p.get<bool>("Verbosity");
    return true;
  }
  
  bool SimpleFlashFinder::analyze(storage_manager* storage)
  {
    
    auto const ophitHandle = storage->get_data<event_ophit>(_producer);

    storage->set_id(storage->run_id(),storage->subrun_id(),storage->event_id());
    
    if(!ophitHandle || ophitHandle->empty()){
      std::cerr<<"\033[93mInvalid Producer name: \033[00m"<<_producer.c_str()<<std::endl;
      throw std::exception();
    }


    std::cout << "0\n";
    // op flash data product to be filled
    event_opflash* opflashes = storage->get_data<event_opflash>(_name);
    std::cout << "1\n";
    // only use the beam-gate window to find flashes
    double beam_gate = 23.4; //usec
    std::cout << "2\n";
    // number of 100ns wide bins
    int n_bins = int(beam_gate / _bin_width)+1;
    std::cout << "3\n";
    
    // collect the total optical charge collected in the hits in each bin
    //std::vector< std::vector<double> > OpCharge(n_bins, std::vector<double>(32,0.));

    // find the peak-tiem for each 100 ns bin
    // this is the time at which the hit with the most PEs is found
    //std::vector<std::pair<double,double> > OpTime(n_bins, std::make_pair(0.,0.) );
    std::cout << "4\n";
    std::vector<std::pair<::larlite::ophit*,double> > OpTime(n_bins, std::make_pair(nullptr,0.) );
    std::cout << "5\n";
    // total number of hits added
    // int nhits = 0;
    std::cout << "6\n";
    //std::vector< std::vector<ophit*> > binned_hits(n_bins,std::vector<ophit*>(ophitHandle->size(),nullptr));
    std::vector< std::vector<ophit*> > binned_hits;
    binned_hits.resize(n_bins);
    
    std::cout << "7\n";
    std::vector< std::vector<double> > OpCharge(n_bins, std::vector<double>(32,0.));
    std::cout << "8\n";
    //bin the ophits
    for(auto& oh : *ophitHandle) {
      std::cout << "9 ";
      //if(oh.PE() < _PE_min_hit) continue;
      double time = oh.PeakTime();
	
      int bin = int(time/_bin_width);

      if( (bin >= n_bins) || (bin < 0) )
	
	continue;
      
      binned_hits[bin].push_back( &oh );
      
    }
    std::cout << "\n10\n";
    //scan each bin. Sum the total charge, find the ophit with the max pe peak, use this as the time of
    //the flash later on.
    for(size_t i=0; i < n_bins; i++) {//loop through bins 
      //how much pe in this bin
      auto total_pe     = TotalCharge(binned_hits[i]);
      auto timed_op_hit = MaxPeakTime(binned_hits[i]);
      OpTime[i] = std::make_pair(timed_op_hit,total_pe);
      
    }
    std::cout << "\n14\n";
    std::map<unsigned,std::vector<unsigned> > connected_bins;

    for( unsigned i = 0; i < n_bins; i++) {

      //no ophits in this bin...
      if ( binned_hits[i].size() <= 0 )
	continue;

      //there are ophits, but no flash or largest hit is too small
      if ( !HaveFlash(OpTime[i]) )
	continue;
      	
      //Hit is here 'fo sho', reserve bins to be connected
      connected_bins[i].reserve(n_bins);

      // Update the channel wise sum for this flash
      UpdatePMTPEs(OpCharge[i],binned_hits[i]);
      
      //We are on the last bin, none left, lets leave
      unsigned b = i;
      if (b + 1 > n_bins - 1)
	break;

      
      //while the next bin has less TOTAL charge than this one...
      while ( OpTime[b+1].second < OpTime[b].second ) {

	// // Flash in the next bin, leave
	// if ( HaveFlash(OpTime[b + 1]) )
	//   break;

	//If next bin is empty, leave
	if ( binned_hits[b+1].size() <= 0 )
	  break;

	//Else gobble up it's charge, mark the bin as taken, sum channel wise pulses
	//OpTime[i].second += TotalCharge(binned_hits[b]);
	UpdatePMTPEs(OpCharge[i],binned_hits[b + 1]);
	
	connected_bins[i].push_back(b + 1);
	
	++b;

	//Next bin is overflow
	if ( b + 1 > n_bins - 1)
	  break;
	
      }
      i = b;
      
      
    }

    
    for( const auto& bin : connected_bins ) {
      auto& b = bin.first;
      
      opflashes->emplace_back( OpTime[b].first->PeakTime(),
			       0.05,
			       OpTime[b].first->PeakTime(),
			       0,
			       OpCharge[b]);

    }
    
    // for(auto const& oh : *ophitHandle)
    //   {
    //   // ignore hits with < X PE
    // 	if(oh.PE() < _PE_min_hit) continue;
	
    //   // allocate this hit to a 100 ns bin
    // 	double time = oh.PeakTime();
	
    // 	int bin = int(time/_bin_width);

    // 	if( (bin >= n_bins) || (bin < 0) )

    // 	  continue;
	
    // 	OpCharge[bin][oh.OpChannel()] += oh.PE();
    // 	nhits +=1;


    // 	// if this hit has the largest charge, adjust the flash's time
    // 	if( OpTime[bin].second < oh.PE() )

    // 	    OpTime[bin] = std::make_pair( oh.PeakTime(), oh.PE() );

    //   } // <-- end loop over ophits

    // // seed bin (bin with the most light)
    // size_t seed = 0;

    // // vector of per-PMT PEs collected
    // std::vector<double> PEperPMT(32,0.);
    
    // // vector of indices to be cleared
    // std::vector<size_t> to_clear;


    // //Candidate flash:    
    
    // for(size_t i=0; i < OpCharge.size()-1; i++) //loop through bins
    //   {

    // 	if( ( TotalCharge(OpCharge.at(i+1) ) > TotalCharge(OpCharge.at(i) ) ) and
    // 	    ( TotalCharge(OpCharge.at(i+1) ) > 0 ) )
    // 	  {
    // 	    seed = i + 1;
    // 	    for(auto& bin : to_clear)
    // 	      {
    // 		OpCharge[bin] = std::vector<double>(32,0.);
    // 		to_clear.clear();
    // 	      }
    // 	  }

    // 	// the next bin is lower than me
    // 	if ( ( TotalCharge(OpCharge.at(i+1) ) < TotalCharge(OpCharge.at(i) ) and
    // 	       ( TotalCharge(OpCharge.at(i+1) ) > 0 ) ) )
    // 	  {
    // 	    // add to each PMT the light from the bin
    // 	    // determined to be "late-light" bin
    // 	    for (size_t pmt=0; pmt < 32; pmt++)
    // 	      {
    // 		OpCharge[seed][pmt] += OpCharge.at(i+1)[pmt];
    // 	      }
    // 	    to_clear.push_back(i+1);
    // 	  }
    //   }
    
    //loop through the 100 ns windows and create flashes
    // for (size_t i=0; i < OpCharge.size(); i++)
    //   {
    // 	if ( TotalCharge(OpCharge.at(i)) <= _PE_min_flash) continue;

    // 	//	We found a flash, good. Lets force number of integration bins
	
    // 	  opflashes->emplace_back( OpTime.at(i).first,
    // 				   0.05,
    // 				   OpTime.at(i).first,
    // 				   0,
    // 				   OpCharge.at(i));
    //   }
    return true;
  }

  double SimpleFlashFinder::TotalCharge(const std::vector<double>& PEs)
  {
    double totalPE = 0.;
    for(auto const& pe : PEs)

      totalPE += pe;
    
    return totalPE;
  }
  
  ::larlite::ophit* SimpleFlashFinder::MaxPeakTime(const std::vector<::larlite::ophit*> & ophits) {

      ::larlite::ophit* largest = nullptr;
      
      auto c = int{-1};
    
      for(auto const& op : ophits) {

	if ( c < 0 ) largest = op;

	if ( largest->PE() < op->PE() )

	  largest = op;

	++c;
      }

      return largest;
    
  }
    
  double SimpleFlashFinder::TotalCharge(const std::vector<::larlite::ophit*>& ophits)
  {
    double totalPE = 0.;
    
    for(auto const& op : ophits) {
      
      totalPE += op->PE();
    }

    return totalPE;
  }

  void SimpleFlashFinder::UpdatePMTPEs(std::vector<double>& pmt_pe, const std::vector<::larlite::ophit*>& channels) {

    //Update the channel PEs total charge for this hit

    // loop over PMTs

    if (channels.size() <= 0)
      throw std::exception();

    for(const auto& channel : channels ) {
      for (int pmt = 0; pmt < 32; pmt++) {

	if ( pmt != channel->OpChannel() ) continue;

	pmt_pe[pmt] += channel->PE();

	break;
      }
    }

  }
  
  bool SimpleFlashFinder::HaveFlash(const std::pair<::larlite::ophit*,double>& optime) {
        
    //This bin has a total PE that is less than necessary
    if ( optime.second < _PE_min_flash )
      return false;

    //This bin has a enough charge, but highest ophit is not largest ophit is not large enough
    if ( optime.first->PE()  < _PE_min_hit ) 
      return false;

    return true;

  }
  
  bool SimpleFlashFinder::finalize()
  {
    return true;
  }

}
#endif
