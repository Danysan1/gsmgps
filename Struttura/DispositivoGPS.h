#ifndef _DISPOSITIVO_GPS
#define _DISPOSITIVO_GPS

struct posizione {
  double latitudine, longitudine;
};

class DispositivoGPS {
  public:
    virtual struct posizione getPosizione() = 0;
};

#endif
