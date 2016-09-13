#ifndef SIMPLEFLASHALGO_CXX
#define SIMPLEFLASHALGO_CXX

#include "SimpleFlashAlgo.h"
#include <set>
namespace pmtana{

  static SimpleFlashAlgoFactory __SimpleFlashAlgoFactoryStaticObject__;

  SimpleFlashAlgo::SimpleFlashAlgo(const std::string name)
    : FlashAlgoBase(name)
  {}
  
  void SimpleFlashAlgo::Configure(const Config_t &p)
  {
    Reset();
    _debug          = p.get<bool>("DebugMode",false);
    _min_pe_flash   = p.get<double>("PEThreshold",10);
    _min_pe_coinc   = p.get<double>("MinPECoinc",  5);
    _min_mult_coinc = p.get<double>("MinMultCoinc", 2);
    _integral_time = p.get<double>("IntegralTime",8);
    _pre_sample    = p.get<double>("PreSample",0.1);
    _veto_time     = p.get<double>("VetoSize",8.);
    _time_res      = p.get<double>("TimeResolution",0.03);
    _pe_baseline_v.clear();
    _pe_baseline_v = p.get<std::vector<double> >("PEBaseline",_pe_baseline_v);
    if(_integral_time > _veto_time) {
      std::cerr << "Integral time cannot exceed veto time!" << std::endl;
      throw std::exception();
    }
    auto const range_start_v = p.get<std::vector<double> >("HitVetoRangeStart");
    auto const range_end_v   = p.get<std::vector<double> >("HitVetoRangeEnd");
    if(range_start_v.size() != range_end_v.size()) {
      std::cerr << "OpHit veto range start and end config value array have different length!" << std::endl;
      throw std::exception();
    }
    for(size_t i=0; i<range_start_v.size(); ++i) {
      if(range_start_v[i] >= range_end_v[i]) {
	std::cerr << "OpHit veto range element " << i
		  << " have start time @ " << range_start_v[i]
		  << " being later than end time @ " << range_end_v[i]
		  << std::endl;
	throw std::exception();
      }
      _flash_veto_range_m.emplace(range_end_v[i],range_start_v[i]);
    }

    auto const ch_list =  p.get<std::vector<size_t> >("OpChannel");
    size_t valid_id=0;
    std::set<size_t> duplicate;
    for(auto const& ch : ch_list) {
      if(ch >= _opch_v.size()) _opch_v.resize(ch+1,-1);
      if(duplicate.find(ch) != duplicate.end()) {
	std::cerr << "Channel number " << ch << " is duplicated!" << std::endl;
	throw std::exception();
      }
      _opch_v[ch] = valid_id;
      valid_id += 1;
      duplicate.insert(ch);
    }

    if(_pe_baseline_v.size() != duplicate.size()) {
      std::cout << "PEBaseline array length (" << _pe_baseline_v.size()
		<< ") is not same as OpDet ID count (" << duplicate.size() << ")!" << std::endl;
      throw std::exception();
    }
  }

  bool SimpleFlashAlgo::Veto(double t) const
  {
    auto iter = _flash_veto_range_m.lower_bound(t);
    if(iter == _flash_veto_range_m.end()) return false;
    return (t >= (*iter).second);
  }

  SimpleFlashAlgo::~SimpleFlashAlgo()
  {}

  LiteOpFlashArray_t SimpleFlashAlgo::RecoFlash(const LiteOpHitArray_t ophits) {

    Reset();

    const size_t NOpDet = _pe_baseline_v.size();

    //static std::vector<double> pesum_v;
    static std::vector<double> mult_v;  //< this is not strictly a multiplicity of PMTs, but multiplicity of hits
    static std::vector<std::vector<double> > pespec_v;
    double min_time=1.1e20;
    double max_time=1.1e20;
    for(auto const& oph : ophits) {
      if(max_time > 1.e20 || oph.peak_time > max_time) max_time = oph.peak_time;
      if(min_time > 1.e20 || oph.peak_time < min_time) min_time = oph.peak_time;
    }
    min_time -= 10* _time_res;
    max_time += 10* _time_res;
    if(_debug)
      std::cout << "T span: " << min_time << " => " << max_time << " ... " << (size_t)((max_time - min_time) / _time_res) << std::endl;

    size_t nbins_pesum_v = (size_t)((max_time - min_time) / _time_res) + 1;
    if(_pesum_v.size()  < nbins_pesum_v) _pesum_v.resize(nbins_pesum_v,0);
    if(mult_v.size()   < nbins_pesum_v) mult_v.resize(nbins_pesum_v,0);
    if(pespec_v.size() < nbins_pesum_v) pespec_v.resize(nbins_pesum_v,std::vector<double>(NOpDet));
    for(size_t i=0; i<_pesum_v.size(); ++i) {
      _pesum_v[i] = 0;
      mult_v[i]  = 0;
      for(auto& v : pespec_v[i]) v=0;
    }

    // Fill _pesum_v
    for(auto const& oph : ophits) {
      if(oph.channel >= _opch_v.size() || _opch_v[oph.channel] < 0) {
	if(_debug) std::cout << "Ignoring OpChannel " << oph.channel << std::endl;
	continue;
      }
      if(Veto(oph.peak_time)) {
	if(_debug) std::cout << "Ignoring hit @ time " << oph.peak_time << std::endl;
	continue;
      }
      size_t index = (size_t)((oph.peak_time - min_time) / _time_res);
      _pesum_v[index] += oph.pe;
      mult_v[index] += 1;
      pespec_v[index][_opch_v[oph.channel]] += oph.pe;
    }

    // Order by pe (above threshold)
    std::map<double,size_t> pesum_idx_map;
    for(size_t idx=0; idx<nbins_pesum_v; ++idx) {
      if(_pesum_v[idx] < _min_pe_coinc   ) continue;
      if(mult_v[idx]  < _min_mult_coinc ) continue;
      pesum_idx_map[1./(_pesum_v[idx])] = idx;
    }

    // Get candidate flash times
    std::vector<std::pair<size_t,size_t> > flash_period_v;
    std::vector<size_t> flash_time_v;
    size_t veto_ctr = (size_t)(_veto_time / _time_res);
    size_t default_integral_ctr = (size_t)(_integral_time / _time_res);
    size_t precount = (size_t)(_pre_sample / _time_res);
    flash_period_v.reserve(pesum_idx_map.size());
    flash_time_v.reserve(pesum_idx_map.size());

    double sum_baseline = 0;
    for(auto const& v : _pe_baseline_v) sum_baseline += v;
    
    for(auto const& pe_idx : pesum_idx_map) {

      //auto const& pe  = 1./(pe_idx.first);
      auto const& idx = pe_idx.second;

      size_t start_time = idx;
      if(start_time < precount) start_time = 0;
      else start_time = idx - precount;

      // see if this idx can be used
      bool skip=false;
      size_t integral_ctr = default_integral_ctr;
      for(auto const& used_period : flash_period_v) {
	if( start_time <= used_period.first && (start_time + veto_ctr) > used_period.first ) {
	  skip=true;
	  break;
	}
	if( used_period.first <= start_time && start_time < (used_period.first + veto_ctr) ) {
	  skip=true;
	  break;
	}
	if( used_period.first >= start_time && used_period.first < (start_time + integral_ctr) ) {
	  if(_debug) std::cout << "Truncating flash @ " << start_time
			       << " (previous flash @ " << used_period.first
			       << ") ... integral ctr change: " << integral_ctr
			       << " => " << used_period.first - start_time << std::endl;
	  
	  integral_ctr = used_period.first - start_time;
	}
	if(_debug) {
	  std::cout << "Flash @ " << min_time + used_period.first * _time_res
		    << " => " << min_time + (used_period.first + used_period.second) * _time_res
		    << " does not interfare with THIS flash @ " << min_time + start_time * _time_res
		    << " => " << min_time + (start_time + integral_ctr) * _time_res << std::endl;
	}
      }
      if(skip) {
	if(_debug) std::cout << "Skipping a candidate @ " << min_time + start_time * _time_res << " as it is in a veto window!" <<std::endl;
	continue;
      }

      // See if this flash is declarable
      double pesum = 0;
      for(size_t i=start_time; i<(start_time+integral_ctr); ++i)

	pesum += _pesum_v[i];

      if(pesum < (_min_pe_flash + sum_baseline)) {
	if(_debug) std::cout << "Skipping a candidate @ " << start_time  << " => " << start_time + integral_ctr
			     << " as it got " << pesum
			     << " PE which is lower than threshold " << (_min_pe_flash + sum_baseline) << std::endl;
	continue;
      }
      
      flash_period_v.push_back(std::pair<size_t,size_t>(start_time,integral_ctr));
      flash_time_v.push_back(idx);
    }

    // Construct flash
    LiteOpFlashArray_t res;
    for(size_t flash_idx=0; flash_idx<flash_period_v.size(); ++flash_idx) {

      auto const& start  = flash_period_v[flash_idx].first;
      auto const& period = flash_period_v[flash_idx].second;
      auto const& time   = flash_time_v[flash_idx];

      std::vector<double> pe_v(NOpDet,0);
      for(size_t index=start; index<(start+period) && index<pespec_v.size(); ++index) {

	for(size_t pmt=0; pmt<NOpDet; ++pmt) pe_v[pmt] += pespec_v[index][pmt];
	  
      }

      for(size_t pmt=0; pmt<NOpDet; ++pmt) {
	pe_v[pmt] -= _pe_baseline_v[pmt];
	if(pe_v[pmt] < 0) pe_v[pmt] = 0.;
      }

      if(_debug) {
	std::cout << "Claiming a flash @ " << min_time + time * _time_res
		  << " : " << std::flush;
	double tmpsum=0;
	for(auto const& v : pe_v) { std::cout << v << " "; tmpsum +=v; }
	std::cout << " ... sum = " << tmpsum << std::endl;
      }

      LiteOpFlash_t flash( min_time + time * _time_res,
			   period * _time_res / 2.,
			   std::move(pe_v) );
      res.emplace_back( std::move(flash) );

    }
    if(_debug) std::cout << std::endl;
    return res;
  }

}
#endif
