/**
 * \file CosmicCalib.h
 *
 * \ingroup App
 * 
 * \brief Class def header for a class CosmicCalib
 *
 * @author kazuhiro
 */

/** \addtogroup App

    @{*/

#ifndef LARLITE_COSMICCALIB_H
#define LARLITE_COSMICCALIB_H

#include "Analysis/ana_base.h"
#include "PECalib.h"
namespace larlite {
  /**
     \class CosmicCalib
     User custom analysis class made by SHELL_USER_NAME
   */
  class CosmicCalib : public ana_base{
  
  public:

    /// Default constructor
    CosmicCalib(){ _name="CosmicCalib"; _fout=0;}

    /// Default destructor
    virtual ~CosmicCalib(){}

    /** IMPLEMENT in CosmicCalib.cc!
        Initialization method to be called before the analysis event loop.
    */ 
    virtual bool initialize();

    /** IMPLEMENT in CosmicCalib.cc! 
        Analyze a data event-by-event  
    */
    virtual bool analyze(storage_manager* storage);

    /** IMPLEMENT in CosmicCalib.cc! 
        Finalize method to be called after all events processed.
    */
    virtual bool finalize();

    void Configure(std::string fname);

  protected:
    std::string _config_file;
    std::string _ophit_producer;
    TTree* _tree;
    double _area;
    double _amp;
    double _pe;
    int    _ch;
    double _twidth;
    double _tpeak;
    pmtana::PECalib _pecalib;
    std::vector<TTree*> _tree_v;
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
