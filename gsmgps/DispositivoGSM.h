#ifndef _DISPOSITIVO_GSM
#define _DISPOSITIVO_GSM

enum risultatoInvioTesto {
  successo,
  testoNullo,
  testoInvalido,
  erroreInterno,
  erroreInvio
};

class DispositivoGSM {
  public:
    virtual enum risultatoInvioTesto inviaTesto(const char* testo) = 0;
};


#endif
