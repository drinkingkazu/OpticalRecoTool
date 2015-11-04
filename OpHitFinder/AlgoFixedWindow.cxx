////////////////////////////////////////////////////////////////////////
//
//  AlgoFixedWindow source
//
////////////////////////////////////////////////////////////////////////

#ifndef ALGOFIXEDWINDOW_CC
#define ALGOFIXEDWINDOW_CC

#include "AlgoFixedWindow.h"

namespace pmtana{

  //################################
  AlgoFixedWindow::AlgoFixedWindow()
  //################################
  {
    Reset();

    _index_start = 0;

    _index_end = 0;
  }

  //###############################################################
  //AlgoFixedWindow::AlgoFixedWindow(const fhicl::ParameterSet &pset)
  AlgoFixedWindow::AlgoFixedWindow(const ::fcllite::PSet & pset)
  //###############################################################
  {
    Reset();

    _index_start = pset.get<size_t>("StartIndex");

    _index_end = pset.get<size_t>("EndIndex");
  }

  //***************************************************************
  AlgoFixedWindow::~AlgoFixedWindow()
  //***************************************************************
  {}

  //***************************************************************
  void AlgoFixedWindow::Reset()
  //***************************************************************
  {
    if(!(_pulse_v.size()))

      _pulse_v.push_back(_pulse);

    _pulse_v[0].reset_param();

  }

  //***************************************************************
  bool AlgoFixedWindow::RecoPulse(const std::vector<short> &wf)
  //***************************************************************
  {
    this->Reset();

    _pulse_v[0].t_start = (double)(_index_start);

    _pulse_v[0].ped_mean  = _ped_mean;

    _pulse_v[0].ped_sigma = _ped_rms;

    if(!_index_end)

      _pulse_v[0].t_end = (double)(wf.size() - 1);

    else

      _pulse_v[0].t_end = (double)_index_end;

    _pulse_v[0].t_max = PMTPulseRecoBase::Max(wf, _pulse_v[0].peak, _index_start, _index_end);

    _pulse_v[0].peak -= _ped_mean;

    PMTPulseRecoBase::Integral(wf, _pulse_v[0].area, _index_start, _index_end);

    _pulse_v[0].area = _pulse_v[0].area - ( _pulse_v[0].t_end - _pulse_v[0].t_start + 1) * _ped_mean;

    return true;

  }

}

#endif
