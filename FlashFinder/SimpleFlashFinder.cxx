//By David C. Ported to larlite by Brooke. ruined by vic in new framework.

#ifndef SIMPLEFLASHFINDER_CXX
#define SIMPLEFLASHFINDER_CXX

#include "SimpleFlashFinder.h"
#include "FhiclLite/ConfigManager.h"

namespace pmtana{

  SimpleFlashFinder::SimpleFlashFinder()
    : FlashFinderBase()
  {}
  
  SimpleFlashFinder::SimpleFlashFinder(const ::fcllite::PSet &p)
    : FlashFinderBase()
  {
    Reset();

    _min_pe_flash   = p.get<double>("PEThreshold",10);
    _min_pe_coinc   = p.get<double>("MinPECoinc",  5);
    _min_mult_coinc = p.get<double>("MinMultCoinc", 2);
    _integral_time = p.get<double>("IntegralTime",8);
    _pre_sample    = p.get<double>("PreSample",0.1);
    _veto_time     = p.get<double>("VetoSize",8.);
    _time_res      = p.get<double>("TimeResolution",0.03);

    if(_integral_time > _veto_time) {
      std::cout << "Integral time cannot exceed veto time!" << std::endl;
      throw std::exception();
    }
  }

  SimpleFlashFinder::~SimpleFlashFinder()
  {}

  unsigned SimpleFlashFinder::Flash(const ::larlite::event_ophit* ophits) {
    Reset();
    const size_t NOpDet=32;
    static std::vector<double> pesum_v;
    static std::vector<double> mult_v;  //< this is not strictly a multiplicity of PMTs, but multiplicity of hits
    static std::vector<std::vector<double> > pespec_v;
    double min_time=1e6;
    double max_time=0;
    for(auto const& oph : *ophits) {
      if(oph.PeakTime() > max_time) max_time = oph.PeakTime();
      if(oph.PeakTime() < min_time) min_time = oph.PeakTime();
    }
    //std::cout << "T span: " << min_time << " => " << max_time << std::endl;
    size_t nbins_pesum_v = (size_t)((max_time - min_time) / _time_res) + 1;
    if(pesum_v.size()  < nbins_pesum_v) pesum_v.resize(nbins_pesum_v,0);
    if(mult_v.size()   < nbins_pesum_v) mult_v.resize(nbins_pesum_v,0);
    if(pespec_v.size() < nbins_pesum_v) pespec_v.resize(nbins_pesum_v,std::vector<double>(NOpDet));
    for(size_t i=0; i<nbins_pesum_v; ++i) {
      pesum_v[i] = 0;
      mult_v[i]  = 0;
      for(auto& v : pespec_v[i]) v=0;
    }

    // Fill pesum_v
    for(auto const& oph : *ophits) {
      if(oph.OpChannel() >= NOpDet) continue;
      size_t index = (size_t)((oph.PeakTime() - min_time) / _time_res);
      pesum_v[index] += oph.PE();
      mult_v[index] += 1;
      pespec_v[index][oph.OpChannel()] += oph.PE();
    }

    // Order by pe (above threshold)
    std::map<double,size_t> pesum_idx_map;
    for(size_t idx=0; idx<nbins_pesum_v; ++idx) {
      if(pesum_v[idx] < _min_pe_coinc   ) continue;
      if(mult_v[idx]  < _min_mult_coinc ) continue;
      pesum_idx_map[1./(pesum_v[idx])] = idx;
    }

    // Get candidate flash times
    std::vector<std::pair<size_t,size_t> > flash_period_v;
    std::vector<size_t> flash_time_v;
    size_t veto_ctr = (size_t)(_veto_time / _time_res);
    size_t default_integral_ctr = (size_t)(_integral_time / _time_res);
    size_t precount = (size_t)(_pre_sample / _time_res);
    flash_period_v.reserve(pesum_idx_map.size());
    flash_time_v.reserve(pesum_idx_map.size());
    for(auto const& pe_idx : pesum_idx_map) {

      auto const& pe  = 1./(pe_idx.first);
      auto const& idx = pe_idx.second;

      size_t start_time = idx;
      if(start_time < precount) start_time = 0;
      else start_time = idx - precount;

      // see if this idx can be used
      bool skip=false;
      size_t integral_ctr = default_integral_ctr;
      for(auto const& used_period : flash_period_v) {
	if( used_period.first < start_time && start_time < (used_period.first + veto_ctr) ) {
	  skip=true;
	  break;
	}
	if( used_period.first < (start_time + integral_ctr) ) {
	  integral_ctr = used_period.first - start_time;
	}
      }
      if(skip) continue;

      // See if this flash is declarable
      double pesum = 0;
      for(size_t i=start_time; i<(start_time+integral_ctr); ++i)

	pesum += pesum_v[i];

      if(pesum < _min_pe_flash) continue;
      
      flash_period_v.push_back(std::pair<size_t,size_t>(start_time,integral_ctr));
      flash_time_v.push_back(idx);
    }

    // Construct flash
    for(size_t flash_idx=0; flash_idx<flash_period_v.size(); ++flash_idx) {

      auto const& start  = flash_period_v[flash_idx].first;
      auto const& period = flash_period_v[flash_idx].second;
      auto const& time   = flash_time_v[flash_idx];

      std::vector<double> pe_v(NOpDet,0);
      for(size_t index=start; index<start+period; ++index) {

	for(size_t pmt=0; pmt<NOpDet; ++pmt) pe_v[pmt] += pespec_v[index][pmt];

      }

      _flash_v.emplace_back( ::larlite::opflash(min_time + time * _time_res, period * _time_res /2.,
						min_time + time * _time_res,
						0,
						pe_v ) );
    }
    return _flash_v.size();
  }

}
#endif

