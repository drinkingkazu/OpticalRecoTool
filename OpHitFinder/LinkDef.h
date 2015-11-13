//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ namespace pmtana+;
#pragma link C++ class pmtana::pulse_param+;
#pragma lunk C++ class pmtana::Waveform_t+;
#pragma lunk C++ class pmtana::PedestalMean_t+;
#pragma lunk C++ class pmtana::PedestalSigma_t+;
#pragma link C++ class std::vector<pmtana::pulse_param>+;

#pragma link C++ class pmtana::PMTPulseRecoBase+;
//#pragma link C++ class pmtana::AlgoPedestal+;
#pragma link C++ class pmtana::AlgoThreshold+;
#pragma link C++ class pmtana::AlgoFixedWindow+;

#pragma link C++ class pmtana::AlgoSlidingWindow+;

#pragma link C++ class pmtana::PulseRecoManager+;

#pragma link C++ class larlite::OpHitFinder+;

#pragma link C++ class pmtana::PMTPedestalBase+;
#pragma link C++ class pmtana::PedAlgoEdges+;
#pragma link C++ class pmtana::PedAlgoRollingMean+;
#pragma link C++ class pmtana::PedAlgoNoContinuation+;

#pragma link C++ class pmtana::PedAlgoUB+;
#pragma link C++ class pmtana::AlgoCFD+;

#pragma link C++ function pmtana::mean(const std::vector<short>&,size_t,size_t)+;
#pragma link C++ function pmtana::std(const std::vector<short>&,const double,size_t,size_t)+;
//ADD_NEW_CLASS ... do not change this line
#endif




