#ifndef _SIM808_GPS
#define _SIM808_GPS

#include "DispositivoGPS.h"

class SIM808GPS : public DispositivoGPS {
  public:
    SIM808GPS ();
    struct posizione getPosizione();
};

#endif
