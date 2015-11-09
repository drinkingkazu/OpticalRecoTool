////////////////////////////////////////////////////////////////////////
//
//  AlgoCFD source
//
////////////////////////////////////////////////////////////////////////

#ifndef ALGOCFD_CXX
#define ALGOCFD_CXX

#include "AlgoCFD.h"
#include "UtilFunc.h"

namespace pmtana{

  //*********************************************************************
  AlgoCFD::AlgoCFD(const std::string name)
    : PMTPulseRecoBase(name)
  //*********************************************************************
  {}

  //*********************************************************************
  //AlgoCFD::AlgoCFD(const fhicl::ParameterSet &pset,
  AlgoCFD::AlgoCFD(const ::fcllite::PSet &pset,
		   const std::string name)
    : PMTPulseRecoBase(name)
      //*********************************************************************
  {

    _F = pset.get<float>("Fraction");
    _D = pset.get<int>  ("Delay");

    Reset();

  }

  //***************************************************************
  AlgoCFD::~AlgoCFD()
  //***************************************************************
  {}

  //***************************************************************
  void AlgoCFD::Reset()
  //***************************************************************
  {
    PMTPulseRecoBase::Reset();
  }

  //***************************************************************
  bool AlgoCFD::RecoPulse(const pmtana::Waveform_t& wf,
			  const pmtana::PedestalMean_t& mean_v,
			  const pmtana::PedestalSigma_t& sigma_v)
  //***************************************************************
  {

    Reset();

    std::vector<double> cfd; cfd.reserve(wf.size());

    // follow cfd procedure: invert waveform, multiply by constant fraction
    // add to delayed waveform.
    for (unsigned k = 0; k < wf.size(); ++k)  {
      
      auto delayed = -1.0 * _F *  ( (float) wf.at(k) - mean_v.at(k) );

      if (k < _D)
	
	cfd.push_back( delayed );

      else

	cfd.push_back(delayed +  ( (float) wf.at(k - _D) - mean_v.at(k) ) );
    }


    // Get the zero point crossings, how can I tell which are meaningful?
    // go to each crossing, see if waveform is above pedestal (high above pedestal)

    auto crossings = LinearZeroPointX(cfd);
    auto number_presample = 5;

    auto peak_thresh  = 5.0;
    auto start_thresh = 5.0;
    auto end_thresh   = 1.5;
    
    double before_mean, after_mean;
    before_mean = after_mean = 0;

    // lambda criteria to determine if inside pulse
    
    auto in_peak = [&wf,&sigma_v,&mean_v](int i, float thresh) -> bool
      { return wf.at(i) > sigma_v.at(i) * thresh +  mean_v.at(i); };

    // loop over CFD crossings
    for(const auto& cross : crossings) {

      //      if( wf.at( cross.first ) > sigma_v.at( cross.first ) * threshold +  mean_v.at( cross.first ) ) {
      if( in_peak( cross.first, peak_thresh) ) {
	_pulse.reset_param();

	int i = cross.first;

	//backwards
	while ( in_peak(i, start_thresh) ){
	  i--;
	  if ( i < 0 ) { i = 0; break; }
	}
	_pulse.t_start = i;
	
	//walk a little further backwards to see if we can get 5 low RMS
	while ( !in_peak(i,start_thresh) ) {
	  if (i == ( _pulse.t_start - number_presample ) ) break;
	  i--;
	  if ( i < 0 ) { i = 0; break; }
	}
	
	before_mean = std::accumulate(std::begin(mean_v) + i,
				      std::begin(mean_v) + _pulse.t_start, 0.0) / ((double) (_pulse.t_start - i));

	i = _pulse.t_start + 1;
	
	//forwards
	while ( in_peak(i,end_thresh) ) {
	  i++;
	  if ( i > wf.size() - 1 ) { i = wf.size() - 1; break; }
	}
		
	_pulse.t_end = i;

	//walk a little further forwards to see if we can get 5 low RMS
	while ( !in_peak(i,end_thresh) ) {
	  if (i == ( _pulse.t_end + number_presample ) ) break;
	  i++;
	  if ( i > wf.size() - 1 ) { i = wf.size() - 1; break; }
	}

	after_mean = std::accumulate(std::begin(mean_v) + _pulse.t_end + 1,
				     std::begin(mean_v) + i + 1, 0.0) / ((double) (i - _pulse.t_end));
	

	//how to decide before or after? set before for now
	_pulse.ped_mean = before_mean;

	if(wf.size() < 50) _pulse.ped_mean = mean_v.front(); //is COSMIC DISCRIMINATOR

	auto it = std::max_element(std::begin(wf) + _pulse.t_start, std::begin(wf) + _pulse.t_end);

	_pulse.t_max      =  it - std::begin(wf);
	_pulse.peak       = *it - _pulse.ped_mean;
	_pulse.t_cdfcross =  cross.second;

	for(auto k = _pulse.t_start; k <= _pulse.t_end; ++k) {
	  auto a = wf.at(i) - _pulse.ped_mean;
	  if ( a > 0 ) _pulse.area += a;
	}
	  
	_pulse_v.push_back(_pulse);
      }
      
    }
    
    
    return true;
    
  }

  // currently returns ALL zero point crossings, we really just want ones associated with peak...
  const std::map<unsigned,double> AlgoCFD::LinearZeroPointX(const std::vector<double>& trace) {

    std::map<unsigned,double> crossing;
    
    //step through the trace and find where slope is POSITIVE across zero
    for ( unsigned i = 0; i < trace.size() - 1; ++i) {

      auto si = ::pmtana::sign(trace.at(i));
      auto sf = ::pmtana::sign(trace.at(i+1));

      if ( si == sf ) //no sign flip, no zero cross
	continue;
       
      if ( sf < si ) //this is a negative slope, continue
	continue;

      //calculate the crossing X based on linear interpolation bt two pts


      crossing[i] = (double) i - trace.at(i) * ( 1.0 / ( trace.at(i+1) - trace.at(i) ) );
      
    }
    

    return crossing;

  }


}

#endif
