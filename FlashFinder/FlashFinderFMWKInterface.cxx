#ifndef __FLASHFINDERFMWKINTERFACE_CXX__
#define __FLASHFINDERFMWKINTERFACE_CXX__

#include "FlashFinderFMWKInterface.h"

namespace pmtana {

  size_t NOpDets()
  { return larutil::Geometry::GetME()->NOpDets(); }

  size_t OpDetFromOpChannel(size_t opch)
  { return larutil::Geometry::GetME()->OpDetFromOpChannel(opch); }

}

#endif
