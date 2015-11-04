////////////////////////////////////////////////////////////////////////
//
//  PMTPulseRecoBase source
//
////////////////////////////////////////////////////////////////////////

#ifndef PMTPULSERECOBASE_CC
#define PMTPULSERECOBASE_CC

#include "PMTPulseRecoBase.h"

namespace pmtana{

  //##################################
  PMTPulseRecoBase::PMTPulseRecoBase()
  //##################################
  {

    _ped_mean = _ped_rms = -1;

    Reset();

  }

  //***************************************************************
  PMTPulseRecoBase::~PMTPulseRecoBase()
  //***************************************************************
  {}

  //***************************************************************
  bool CheckIndex(const std::vector<short> &wf, const size_t &begin, size_t &end)
  //***************************************************************
  {
    if(begin >= wf.size() || end >= wf.size() || begin > end){

      std::cerr << Form("Invalid arguments: waveform length = %zu, begin = %zu, end = %zu!",
			wf.size(), begin, end)
		<< std::endl;
      return false;

    }

    if(!end) end = wf.size() - 1;

    return true;
  }

  //***************************************************************
  void PMTPulseRecoBase::Reset()
  //***************************************************************
  {
    _pulse.reset_param();

    _pulse_v.clear();

    _pulse_v.reserve(3);
  
  }

  //***************************************************************
  const pulse_param& PMTPulseRecoBase::GetPulse(size_t index) const
  //***************************************************************
  {

    if(index >= _pulse_v.size()) {
      
      std::cerr << "\033[93m"
		<< "Invalid pulse index: " << index
		<< "\033[00m"
		<< std::endl;

      throw std::exception();
    }

    else return _pulse_v.at(index);

  }

  //***************************************************************
  bool PMTPulseRecoBase::Integral(const std::vector<short> &wf,
				  double &result,
				  size_t begin,
				  size_t end) const
  //***************************************************************
  {
  
    if(!CheckIndex(wf,begin,end)) return false;
  
    std::vector<short>::const_iterator begin_iter(wf.begin());
  
    std::vector<short>::const_iterator end_iter(wf.begin());

    begin_iter = begin_iter + begin;

    end_iter = end_iter + end + 1;
  
    result = (double)(std::accumulate(begin_iter, end_iter, 0));

    return true;
  }

  //***************************************************************
  bool PMTPulseRecoBase::Derivative(const std::vector<short> &wf,
				    std::vector<int32_t> &diff,
				    size_t begin,
				    size_t end) const 
  //***************************************************************
  {

    if(CheckIndex(wf,begin,end)){
    
      diff.clear();
      diff.reserve(end - begin);

      for(size_t index = begin ; index <= end ; ++index)
      
	diff.push_back(wf.at(index+1) - wf.at(index));

      return true;
    }

    return false;

  }

  //***************************************************************
  size_t PMTPulseRecoBase::Max(const std::vector<short> &wf,
			       double &result,
			       size_t begin,
			       size_t end) const
  //***************************************************************
  {

    size_t target_index = wf.size() + 1;

    result = 0;

    if(CheckIndex(wf,begin,end)) {

      for(size_t index = begin; index <= end; ++index)
      
	if( result < wf.at(index)) { target_index = index; result = (double)(wf.at(index)); }
    
    }

    return target_index;

  }

  //***************************************************************
  size_t PMTPulseRecoBase::Min(const std::vector<short> &wf,
			       double &result,
			       size_t begin,
			       size_t end) const
  //***************************************************************
  {

    size_t target_index = wf.size() + 1;

    result = 4096;

    if(CheckIndex(wf,begin,end)) {

      for(size_t index = begin; index <= end; ++index)
      
	if( result > wf.at(index)) { target_index = index; result = (double)(wf.at(index)); }
    
    }

    return target_index;

  }
}
#endif

