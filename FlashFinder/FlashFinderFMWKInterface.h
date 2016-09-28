#ifndef __FLASHFINDERFMWKINTERFACE_H__
#define __FLASHFINDERFMWKINTERFACE_H__

#include "FhiclLite/ConfigManager.h"
#include "LArUtil/Geometry.h"
namespace pmtana {

  typedef ::fcllite::PSet Config_t;

  size_t NOpDets();

  size_t OpDetFromOpChannel(size_t opch);

}
#endif
