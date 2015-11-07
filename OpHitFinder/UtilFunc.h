#ifndef larana_OPTICALDETECTOR_UTILFUNC_H
#define larana_OPTICALDETECTOR_UTILFUNC_H

#include "OpticalRecoTypes.h"

namespace pmtana {

  double mean(const std::vector<short>& wf, size_t start=0, size_t nsample=0);
  
  double std(const std::vector<short>& wf, const double ped_mean, size_t start=0, size_t nsample=0);

  double BinnedMaxOccurrence(const PedestalMean_t& mean_v,const size_t nbins);

}

#endif
