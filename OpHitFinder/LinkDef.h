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
#pragma link C++ class std::vector<pmtana::pulse_param>+;
#pragma link C++ class pmtana::PMTPulseRecoBase+;
#pragma link C++ class pmtana::AlgoPedestal+;
#pragma link C++ class pmtana::AlgoThreshold+;
#pragma link C++ class pmtana::AlgoFixedWindow+;
#pragma link C++ class pmtana::AlgoSlidingWindow+;
#pragma link C++ class pmtana::PulseRecoManager+;
#pragma link C++ class larlite::OpHitFinder+;
//ADD_NEW_CLASS ... do not change this line
#endif



