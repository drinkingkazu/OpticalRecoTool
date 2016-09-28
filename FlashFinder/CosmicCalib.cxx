#ifndef LARLITE_COSMICCALIB_CXX
#define LARLITE_COSMICCALIB_CXX

#include "CosmicCalib.h"
#include "DataFormat/ophit.h"
#include "FhiclLite/ConfigManager.h"
namespace larlite {

  void CosmicCalib::Configure(std::string fname)
  {

    ::fcllite::ConfigManager cfg_mgr;
    
    cfg_mgr.AddCfgFile(fname);
    
    auto const& main_cfg = cfg_mgr.Config();
    
    auto const p = main_cfg.get<pmtana::Config_t>("CosmicCalib");

    _ophit_producer = p.get<std::string>("OpHitProducer");

    _pecalib.Configure(main_cfg.get<pmtana::Config_t>(p.get<std::string>("PECalib")));

  }
  
  bool CosmicCalib::initialize() {

    _tree_v.clear();
    for(size_t ch=0; ch<32; ++ch) {
      auto tree = new TTree(Form("tree%02zu",ch),"");
      tree->Branch("area",&_area,"area/D");
      tree->Branch("amp",&_amp,"amp/D");
      tree->Branch("pe",&_pe,"pe/D");
      tree->Branch("ch",&_ch,"ch/I");
      tree->Branch("twidth",&_twidth,"twidth/D");
      tree->Branch("tpeak",&_tpeak,"tpeak/D");
      _tree_v.push_back(tree);
    }
    return true;
  }
  
  bool CosmicCalib::analyze(storage_manager* storage) {

    auto ev_ophit = storage->get_data<event_ophit>(_ophit_producer);

    for(auto const& oph : *ev_ophit) {
      _ch = oph.OpChannel();
      if(_ch > 231 || _ch < 200)  continue;
      _tpeak = oph.PeakTime();
      if(0.0 < _tpeak && _tpeak < 30) continue;
      _amp = oph.Amplitude();
      _area = oph.Area();

      auto opdet = ::pmtana::OpDetFromOpChannel(_ch);
      _pe = _pecalib.CosmicPE(opdet,oph.Area(),oph.Amplitude());

      _twidth = oph.Width();
      _tree_v[_ch-200]->Fill();
    }
  
    return true;
  }

  bool CosmicCalib::finalize() {

    if(_fout) {

      _fout->cd();
      for(auto& t : _tree_v) t->Write();

    }
  
    return true;
  }

}
#endif
