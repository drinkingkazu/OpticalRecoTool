//by vic, based on David C. simple flash idea.

#ifndef NOTSIMPLEFLASHFINDER_H
#define NOTSIMPLEFLASHFINDER_H

namespace pmtana
{

  class NotSimpleFlashFinder : FlashFinderBase{

  public:

    NotSimpleFlashFinder();

    virtual ~NotSimpleFlashFinder();

      
  protected:
    
  private:

    double TotalCharge(const std::vector<double>& PEs);
    
    double _min_time_integrated;
    
    
    // minimum PE to make a flash
    double _PE_min_flash;

    // minimum PE to use an OpHit
    double _PE_min_hit;

    // bin-width in time
    double _bin_width;
    



  };

}
#endif

/** @} */ // end of doxygen group
