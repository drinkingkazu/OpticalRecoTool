//
// cint script to generate libraries
// Declaire namespace & classes you defined
// #pragma statement: order matters! Google it ;)
//

#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//#pragma link C++ class sample+;
//#pragma link C++ class larlite::SimpleFlashFinder+;

#pragma link C++ namespace pmtana+;
#pragma link C++ class larlite::FlashFinder+;
#pragma link C++ class larlite::MCFlashFinder+;
#pragma link C++ class larlite::CheatFlashFinder+;
#pragma link C++ class larlite::FlashComp+;
#pragma link C++ class larlite::CosmicCalib+;
//ADD_NEW_CLASS ... do not change this line

#pragma link C++ class pmtana::FlashFinderManager+;

//#pragma link C++ class pmtana::FlashAlgoBase+;
//#pragma link C++ class pmtana::SimpleFlashAlgo+;
//#pragma link C++ class pmtana::NotSimpleFlashFinder+;

#endif


