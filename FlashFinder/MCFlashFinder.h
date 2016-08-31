/**
 * \file MCFlashFinder.h
 *
 * \ingroup MCFlash
 * 
 * \brief Class def header for a class MCFlashFinder
 *
 * @author kazuhiro
 */

/** \addtogroup MCFlash

    @{*/

#ifndef LARLITE_MCFLASHFINDER_H
#define LARLITE_MCFLASHFINDER_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class MCFlashFinder
     User custom analysis class made by SHELL_USER_NAME
   */
  class MCFlashFinder : public ana_base{
  
  public:

    /// Default constructor
    MCFlashFinder(){ _name="MCFlashFinder"; _fout=0;}

    /// Default destructor
    virtual ~MCFlashFinder(){}

    /** IMPLEMENT in MCFlashFinder.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in MCFlashFinder.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in MCFlashFinder.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

  protected:

  };
}
#endif

//**************************************************************************
// 
// For Analysis framework documentation, read Manual.pdf here:
//
// http://microboone-docdb.fnal.gov:8080/cgi-bin/ShowDocument?docid=3183
//
//**************************************************************************

/** @} */ // end of doxygen group 
