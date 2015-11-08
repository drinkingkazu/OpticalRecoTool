////////////////////////////////////////////////////////////////////////
//
//  PedAlgoRollingMean source
//
////////////////////////////////////////////////////////////////////////

#ifndef larana_OPTICALDETECTOR_PEDALGOROLLINGMEAN_CXX
#define larana_OPTICALDETECTOR_PEDALGOROLLINGMEAN_CXX

#include "PedAlgoRollingMean.h"
#include "UtilFunc.h"

#include <ctime>

#include "TH1D.h"

namespace pmtana{

  //*****************************************************************
  PedAlgoRollingMean::PedAlgoRollingMean(const std::string name)
    : PMTPedestalBase(name)
  //*****************************************************************
  {}

  //**************************************************************************
  //PedAlgoRollingMean::PedAlgoRollingMean(const fhicl::ParameterSet &pset,
  PedAlgoRollingMean::PedAlgoRollingMean(const ::fcllite::PSet &pset,
					     const std::string name)
    : PMTPedestalBase(name)
  //############################################################
  {

    _sample_size = pset.get<size_t>("SampleSize");

    _max_sigma = pset.get<float>("MaxSigma");

    _ped_range_max = pset.get<float>("PedRangeMax");

    _ped_range_min = pset.get<float>("PedRangeMin");
        
    srand(static_cast<unsigned int>(time(0)));
 
  }

  //*******************************************
  PedAlgoRollingMean::~PedAlgoRollingMean()
  //*******************************************
  {}

  //****************************************************************************
  bool PedAlgoRollingMean::ComputePedestal( const pmtana::Waveform_t& wf,
					      pmtana::PedestalMean_t&   mean_v,
					      pmtana::PedestalSigma_t&  sigma_v)
  //****************************************************************************
  {
    // parameters
    if(wf.size()<=(_sample_size * 2))
      return false;

    const size_t window_size = _sample_size*2;

    // middle mean
    for(size_t i=0; i< wf.size(); ++i) {

      mean_v[i]  = 0;
      sigma_v[i] = 0;

      if( i < _sample_size || i >= (wf.size() - _sample_size) ) continue;

      mean_v[i]  = mean (wf,i - _sample_size,window_size);
      sigma_v[i] = std  (wf,mean_v[i],i - _sample_size,window_size);
    }

    // front mean
    for(size_t i=0; i<_sample_size; ++i) {

      mean_v[i]  = mean_v[_sample_size];
      sigma_v[i] = sigma_v[_sample_size];
      
    }
    // tail mean
    for(size_t i=(wf.size() - _sample_size); i<wf.size(); ++i) {

      mean_v[i]  = mean_v[wf.size() - _sample_size -1];
      sigma_v[i] = sigma_v[wf.size() - _sample_size -1];

    }

    unsigned nbins = 1000;  

    //////////////////seg faulting...
    // const auto mode_mean  = BinnedMaxOccurrence(mean_v ,nbins);
    // const auto mode_sigma = BinnedMaxOccurrence(sigma_v,nbins);

    auto mode_mean  = get_max(mean_v,nbins);
    auto mode_sigma = get_max(sigma_v,nbins);

    double threshold = 4;
    
    size_t i = 0;
    int    s = 0;

    unsigned n_samples_before = 5; // might use?

    while ( i < wf.size() ) {
      
      auto const& mean  = mean_v[i]; 
      auto const& sigma = sigma_v[i];

      // check if current sigma and mean are "good"
      if(sigma <= threshold * mode_sigma && fabs(mean - mode_mean) <= threshold * mode_sigma) {

	//wasn't good in past, lets catch up and fix as we go
    	while ( s < i ) {
    	  mean_v [s] = floor(mode_mean) + (double) ( rand() % (unsigned) 5) / 4.0;
    	  sigma_v[s] = mode_sigma;
	  
    	  s++;
    	}

    	s = i;
    	++s;
      }
      
      i++;
    }
  

    return true;

  }
  const double PedAlgoRollingMean::get_max(const std::vector<double>& v ,int bins) const {

    auto max_it = std::max_element(std::begin(v), std::end(v));
    auto min_it = std::min_element(std::begin(v), std::end(v));

    TH1D th("th",";;",bins,*min_it,*max_it);

    for (const auto & m : v) th.Fill(m);

    return th.GetXaxis()->GetBinCenter(th.GetMaximumBin());
    
  }
}

#endif
