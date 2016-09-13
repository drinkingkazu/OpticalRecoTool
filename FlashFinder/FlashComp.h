/**
 * \file FlashComp.h
 *
 * \ingroup FlashComp
 * 
 * \brief Class def header for a class FlashComp
 *
 * @author kazuhiro
 */

/** \addtogroup FlashComp

    @{*/

#ifndef LARLITE_FLASHCOMP_H
#define LARLITE_FLASHCOMP_H

#include "Analysis/ana_base.h"

namespace larlite {
  /**
     \class FlashComp
     User custom analysis class made by SHELL_USER_NAME
   */
  class FlashComp : public ana_base{
  
  public:

    /// Default constructor
    FlashComp(){ _name="FlashComp"; _fout=0;}

    /// Default destructor
    virtual ~FlashComp(){}

    /** IMPLEMENT in FlashComp.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in FlashComp.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in FlashComp.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void TreePrefix(const std::string name) { _tree_prefix = name; }

    void SetProducers(const std::string cheat, const std::string opflash, const std::string simple)
    { _cheat_producer = cheat; _opflash_producer = opflash; _simple_producer = simple; }

  protected:

    std::string _tree_prefix;
    std::string _simple_producer, _opflash_producer, _cheat_producer;
    
    TTree* _flash_tree;

    int _bnb;

    double _mcflash_pe;

    double _cheat_pe;
    double _opflash_pe;
    double _simple_pe;

    double _mcflash_t;
    double _cheat_dt;
    double _opflash_dt;
    double _simple_dt;

    TTree* _event_tree;
    int _mcflash_ctr;
    int _cheat_ctr;
    int _opflash_ctr;
    int _simple_ctr;

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
