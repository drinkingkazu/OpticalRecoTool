/**
 * \file CheatFlashFinder.h
 *
 * \ingroup FlashFinder
 * 
 * \brief Class def header for a class CheatFlashFinder
 *
 * @author kazuhiro
 */

/** \addtogroup FlashFinder

    @{*/

#ifndef LARLITE_CHEATFLASHFINDER_H
#define LARLITE_CHEATFLASHFINDER_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class CheatFlashFinder
     User custom analysis class made by SHELL_USER_NAME
   */
  class CheatFlashFinder : public ana_base{
  
  public:

    /// Default constructor
    CheatFlashFinder(){ _name="CheatFlashFinder"; _fout=0;}

    /// Default destructor
    virtual ~CheatFlashFinder(){}

    /** IMPLEMENT in CheatFlashFinder.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in CheatFlashFinder.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in CheatFlashFinder.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void SetIOProducer(const std::string ophit, const std::string opflash)
    { _ophit_producer = ophit; _opflash_producer = opflash; }

    void EmulateFEM(bool doit) { _emulate_fem = doit; }

  protected:
    bool _emulate_fem;
    std::string _ophit_producer;
    std::string _opflash_producer;
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
