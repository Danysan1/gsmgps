#ifndef _SIM808_GSM
#define _SIM808_GSM

#include "DispositivoGSM.h"

class SIM808GSM : public DispositivoGSM {
  public:
    SIM808GSM();
    enum risultatoInvioTesto inviaTesto(char* testo);
};

#endif
