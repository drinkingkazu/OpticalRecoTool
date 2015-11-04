////////////////////////////////////////////////////////////////////////
//
//  AlgoPedestal source
//
////////////////////////////////////////////////////////////////////////

#ifndef ALGOPEDESTAL_CC
#define ALGOPEDESTAL_CC

#include "AlgoPedestal.h"

namespace pmtana{

  //##########################
  AlgoPedestal::AlgoPedestal()
  //##########################
  {

    _mean  = -1;

    _sigma = -1;

  }

  //#########################################################
  AlgoPedestal::~AlgoPedestal()
  //#########################################################
  {}

  //*************************************************************************************************
  void AlgoPedestal::ComputePedestal(const std::vector<short>& wf, size_t start, size_t nsample) const
  //*************************************************************************************************
  {  
    _mean  = -1;
    _sigma = -1;
    // GVS bugfix
    //_mean  = 0.0;
    //_sigma = 0.0;
  
    if( (start + nsample) > wf.size() ){
      std::cout << Form("Wavelength too short (%zu ADC samples) to compute pedestal! (minimum %zu)",
			wf.size(),(start + nsample)) << std::endl;
      return;
    }
    
    for(size_t index=start; index < (start + nsample); ++index)

      _mean += wf.at(index);

    _mean = _mean / ((double)nsample);

    for(size_t index=0; index < (start+nsample); ++index)
    
      _sigma += pow( (wf.at(index) - _mean), 2 );

    _sigma = sqrt(_sigma/((double)(nsample)));

    return;

  }

}

#endif
