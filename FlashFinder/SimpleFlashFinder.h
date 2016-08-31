//By David C. Ported to larlite by Brooke. ruined by vic in new framework.

#ifndef SIMPLEFLASHFINDER_H
#define SIMPLEFLASHFINDER_H

#include "FlashFinderBase.h"
#include "FhiclLite/ConfigManager.h"

namespace pmtana
{

  class SimpleFlashFinder : public FlashFinderBase {

  public:

    SimpleFlashFinder();

    SimpleFlashFinder(const ::fcllite::PSet &p);
    
    virtual ~SimpleFlashFinder();

  private:

    double TotalCharge(const std::vector<double>& PEs);

    // minimum PE to make a flash
    double _min_pe_flash;

    // minimum PE to make a flash candidate
    double _min_pe_coinc;

    // minimum PE to use an OpHit
    double _min_mult_coinc;

    // integral period
    double _integral_time;

    // veto time
    double _veto_time;

    // time resolution of pe sum
    double _time_res;

    // time pre-sample
    double _pre_sample;

    // pw aum array
    std::vector<double> _pesum_v;
      
  protected:

    unsigned Flash(const ::larlite::event_ophit* ophits);
    
  };

}
#endif

/** @} */ // end of doxygen group
