//by vic, based on David C. simple flash idea.

#ifndef NOTSIMPLEFLASHFINDER_CXX
#define NOTSIMPLEFLASHFINDER_CXX

#include "NotSimpleFlashFinder.h"
#include "FhiclLite/ConfigManager.h"

namespace pmtana{

  NotSimpleFlashFinder::NotSimpleFlashFinder()
    : FlashFinderBase()
  {}
  
  NotSimpleFlashFinder::NotSimpleFlashFinder(const ::fcllite::PSet &p)
    : FlashFinderBase()
  {

    _min_time_integrated = p.get<double>("MinTimeIntegrated");
  
    _PE_min_flash = 10; //50;
    _PE_min_hit = 1;
    _bin_width = 0.1; // usec

  }

  NotSimpleFlashFinder::~NotSimpleFlashFinder()
  {}

  unsigned NotSimpleFlashFinder::Flash(const std::vector<::larlite::ophit>& ophits) {
    
    
     // only use the beam-gate window to find flashes
    double beam_gate = 23.4; //usec
    std::cout << "2\n";
    // number of 100ns wide bins
    int n_bins = int(beam_gate / _bin_width)+1;
    std::cout << "3\n";

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
    for(auto& oh : *ophits) {
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
    
    return _flash_v.size();
    
    
  }
    
  double NotSimpleFlashFinder::TotalCharge(const std::vector<double>& PEs)
  {
    double totalPE = 0.;
    for(auto const& pe : PEs)

      totalPE += pe;
    
    return totalPE;
  }
  
  ::larlite::ophit* NotSimpleFlashFinder::MaxPeakTime(const std::vector<::larlite::ophit*> & ophits) {

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
    
  double NotSimpleFlashFinder::TotalCharge(const std::vector<::larlite::ophit*>& ophits)
  {
    double totalPE = 0.;
    
    for(auto const& op : ophits) {
      
      totalPE += op->PE();
    }

    return totalPE;
  }

  void NotSimpleFlashFinder::UpdatePMTPEs(std::vector<double>& pmt_pe, const std::vector<::larlite::ophit*>& channels) {

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
  
  bool NotSimpleFlashFinder::HaveFlash(const std::pair<::larlite::ophit*,double>& optime) {
        
    //This bin has a total PE that is less than necessary
    if ( optime.second < _PE_min_flash )
      return false;

    //This bin has a enough charge, but highest ophit is not largest ophit is not large enough
    if ( optime.first->PE()  < _PE_min_hit ) 
      return false;

    return true;

  }
  

}
#endif

