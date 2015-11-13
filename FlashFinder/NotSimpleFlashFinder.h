//by vic, based on David C. simple flash idea.

#ifndef NOTSIMPLEFLASHFINDER_H
#define NOTSIMPLEFLASHFINDER_H

#include "FlashFinderBase.h"
#include "FhiclLite/ConfigManager.h"

namespace pmtana
{

  class NotSimpleFlashFinder : public FlashFinderBase {

  public:

    NotSimpleFlashFinder();

    NotSimpleFlashFinder(const ::fcllite::PSet &p);
    
    virtual ~NotSimpleFlashFinder();   
    
  private:

    double TotalCharge(const std::vector<double>& PEs);

    double TotalCharge(const std::vector<const ::larlite::ophit*>& ophits);

    void   UpdatePMTPEs(std::vector<double>& pmt_pe,
		        const std::vector<const ::larlite::ophit*>& channels);
    
    bool HaveFlash(const std::pair<const ::larlite::ophit*,double>& optime);

    const ::larlite::ophit* MaxPeakTime(const std::vector<const ::larlite::ophit*> & ophits);
    
    double _min_time_integrated;
    
    // minimum PE to make a flash
    double _PE_min_flash;

    // minimum PE to use an OpHit
    double _PE_min_hit;

    // bin-width in time
    double _bin_width;
    
  protected:

    unsigned Flash(const ::larlite::event_ophit* ophits);


  };

}
#endif

/** @} */ // end of doxygen group
