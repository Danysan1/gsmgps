#ifndef _SIM808
#define _SIM808

#include "DispositivoGSM.h"
#include "DispositivoGPS.h"

class SIM808 : public DispositivoGSM, public DispositivoGPS {
  public:
    SIM808();
    enum risultatoInvioTesto inviaTesto(const char* testo);
    struct posizione getPosizione();
};

#endif
