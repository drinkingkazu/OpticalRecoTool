#ifndef LARLITE_SIMPLEFLASHFINDER_CXX
#define LARLITE_SIMPLEFLASHFINDER_CXX

#include "SimpleFlashFinder.h"

#include "DataFormat/ophit.h"
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
    _producer = p.get<std::string>("OpFlashProducer");
  
    _PE_min_flash = 10; //50;
    _PE_min_hit = 1;
    _bin_width = 0.1; // usec

    _verbose = p.get<bool>("Verbosity");
    return true;
  }
  
  bool SimpleFlashFinder::analyze(storage_manager* storage)
  {

    //auto const ophitHandle = storage->get_data<event_ophit>(_producer);
    auto const ophitHandle = storage->get_data<event_ophit>("OpHitFinder");

    storage->set_id(storage->run_id(),storage->subrun_id(),storage->event_id());
    //std::cout << ophitHandle->size() << std::endl;
    
    if(!ophitHandle || ophitHandle->empty()){
      //std::cerr<<"\033[93mInvalid Producer name: \033[00m"<<_producer.c_str()<<std::endl;
      std::cerr<<"\033[93mInvalid Producer name: \033[00m"<<"OpHitFinder"<<std::endl;
      throw std::exception();
    }

    // op flash data product to be filled
    event_opflash* opflashes = storage->get_data<event_opflash>(_name);

    // only use the beam-gate window to find flashes
    double beam_gate = 23.4; //usec

    // number of 100ns wide bins
    int n_bins = int(beam_gate/_bin_width)+1;

    // collect the total optical charge collected in the hits in each bin
    std::vector< std::vector<double> > OpCharge(n_bins, std::vector<double>(32,0.));

    // find the peak-tiem for each 100 ns bin
    // this is the time at which the hit with the most PEs is found
    std::vector<std::pair<double,double> > OpTime(n_bins, std::make_pair(0.,0.) );

    // total number of hits added
    int nhits = 0;
    
    for(auto const& oh : *ophitHandle)
      {
      // ignore hits with < 5 PE
      if(oh.PE() < _PE_min_hit) continue;

      // allocate this hit to a 100 ns bin
      double time = oh.PeakTime();

      int bin = int(time/_bin_width);

      if( (bin >= n_bins) || (bin < 0) )
	{
	  // std::cout << "We should not hav a hit at this time. Something is wrong!" << std::endl;
	  continue;
	}

      OpCharge[bin][oh.OpChannel()] += oh.PE();
      nhits +=1;

      // if this hit has the largest charge, adjust the flash's time
      if( OpTime[bin].second < oh.PE() )
	{
	OpTime[bin] = std::make_pair(oh.PeakTime(), oh.PE() );
	}
      } // <-- end loop over ophits

    // seed bin (bin with the most light)
    size_t seed = 0;

    // vector of per-PMT PEs collected
    std::vector<double> PEperPMT(32,0.);

    // vecotr of indices to be cleared
    std::vector<size_t> to_clear;

    for(size_t i=0; i < OpCharge.size()-1; i++)
      {
      if( ( TotalCharge(OpCharge.at(i+1) ) > TotalCharge(OpCharge.at(i) ) ) and
	  ( TotalCharge(OpCharge.at(i+1) ) > 0 ) )
	{
	  seed = i+1;
	  for(auto& bin : to_clear)
	    {
	      OpCharge[bin] = std::vector<double>(32,0.);
	      to_clear.clear();
	    }
	}
      // if a bin is lower than the previous one
      if ( ( TotalCharge(OpCharge.at(i+1) ) < TotalCharge(OpCharge.at(i) ) and
	     ( TotalCharge(OpCharge.at(i+1) ) > 0) ) )
	{
	  // add to each PMT the light from the bin
	  //determined to be "late-light" bin
	  for (size_t pmt=0; pmt < 32; pmt++)
	    {
	      OpCharge[seed][pmt] += OpCharge.at(i+1)[pmt];
	    }
	  to_clear.push_back(i+1);
	}
      }
    
    // loop through the 100 ns windows and create flashes
    for (size_t i=0; i < OpCharge.size(); i++)
      {
	if ( TotalCharge(OpCharge.at(i)) <= _PE_min_flash) continue;
	
	opflashes->emplace_back( OpTime.at(i).first,
				 0.05,
				 OpTime.at(i).first,
				 0,
				 OpCharge.at(i));
      }
    return true;
  }

  double SimpleFlashFinder::TotalCharge(const std::vector<double>& PEs)
  {
    double totalPE = 0.;
    for(auto const& pe : PEs)
      {
	totalPE += pe;
      }
    return totalPE;
  }

  bool SimpleFlashFinder::finalize()
  {
    return true;
  }

}
#endif
