// https://www.elecrow.com/wiki/index.php?title=SIM808_GPRS/GSM%2BGPS_Shield_v1.1

#include "DispositivoGPS.h"
#include "DispositivoGSM.h"
#include "SIM808.h"

DispositivoGPS* gps;
DispositivoGSM* gsm;

void setup() {
  SIM808* s = new SIM808();
  gsm = s;
  gps = s;
}

void loop() {
  // put your main code here, to run repeatedly:

}

void inviaGPS() {
  struct posizione pos = gps->getPosizione();
  const char* testo = "TODO";
  enum risultatoInvioTesto ris = gsm->inviaTesto(testo);
}
