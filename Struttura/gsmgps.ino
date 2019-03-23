// https://www.elecrow.com/wiki/index.php?title=SIM808_GPRS/GSM%2BGPS_Shield_v1.1

#include "DispositivoGPS.h"
#include "DispositivoGSM.h"
#include "SIM808GPS.h"
#include "SIM808GSM.h"

DispositivoGPS* gps;
DispositivoGSM* gsm;

void setup() {
  gsm = new SIM808GSM();
  gps = new SIM808GPS();
}

void loop() {
  // put your main code here, to run repeatedly:

}

void inviaGPS(){
  struct posizione pos = gps->getPosizione();
  char* testo = "TODO";
  enum risultatoInvioTesto ris = gsm->inviaTesto(testo);
  }
