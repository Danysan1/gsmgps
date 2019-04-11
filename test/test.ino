// Gestione SIM808
//
// Documentazione shield:
// https://www.elecrow.com/wiki/index.php?title=SIM808_GPRS/GSM%2BGPS_Shield_v1.1
//
// Connessione:
// ###### TX         RX ########### TX3       RX ##########
// #    #---------------#         #--------------#        #
// # PC # USB    Serial # Arduino # Serial3      # SIM808 #
// #    #---------------#         #--------------#        #
// ###### RX         TX ########### RX3       TX ##########
//
// Comunicazione Arduino-SIM808: Protocollo M2M AT:
// Comunicazione seriale basata su comandi testuali che iniziano con AT
// https://www.elecrow.com/wiki/index.php?title=SIM808_GPRS/GSM%2BGPS_Shield_v1.1#Set_Baud_and_Enable_Charging_Function
// https://doc.qt.io/archives/qtextended4.4/atcommands.html
// https://github.com/SeeedDocument/Mini-GSM-GPRS-GPS-Breakout-SIM808/raw/master/res/SIM800_ATCommand_Manual_V1.02.pdf
// http://wiki.seeedstudio.com/Mini_GSM_GPRS_GPS_Breakout_SIM808/#set-baud-and-enable-charging-function
// https://m2msupport.net/m2msupport/sms-at-commands/
//
// Esiste una libreria che si occupa di tutto questo, l'ho testata nello sketch AllFunctions ma non l'ho usata qui
// https://github.com/adafruit/Adafruit_FONA
// https://learn.adafruit.com/adafruit-fona-808-cellular-plus-gps-breakout/overview

#include <stdio.h>
#include <stdlib.h>

#define BAUD_RATE 9600

#define POWER_PIN 9 // Numero del pin per l'accensione/spegnimento della shield
#define GSMSerial Serial3 // Seriale usata per comunicare con la shield

#define PIN_SIM 7975 // Pin della sim

#define DESTINATARIO "+393471840366"
//#define DESTINATARIO "+393490507236"

struct posizione {
  double latitudine, longitudine;
};

struct sms {
#define SMS_NUMERO_DIM 16
#define SMS_PAYLOAD_DIM 160
  char numero[SMS_NUMERO_DIM], payload[SMS_PAYLOAD_DIM];
};

void setup() {
  Serial.begin(BAUD_RATE);
  GSMSerial.begin(BAUD_RATE);
  pinMode(POWER_PIN, OUTPUT);
  Serial.println("| Comandi:");
  Serial.println("| 0 --> Accendi/Spegni");
  Serial.println("| 1 --> Verifica connessione");
  Serial.println("| 2 --> Imposta debug verboso");
  Serial.println("| $ --> Controlla stato PIN");
  Serial.println("| % --> Configura PIN");
  Serial.println("| 3 --> Stampa qualità GSM");
  Serial.println("| 4 --> Invia SMS");
  Serial.println("| 5 --> Leggi SMS");
  Serial.println("| / --> Stampa SMS");
  Serial.println("| ( --> Rimuovi SMS");
  Serial.println("| 6 --> Esegui chiamata");
  Serial.println("| 7 --> Avvia GPS");
  Serial.println("| = --> Stampa qualità GPS");
  Serial.println("| 8 --> Invia posizione GPS");
  Serial.println("| ? --> Stampa posizione GPS");
  Serial.println("| 9 --> Spegni GPS");
  Serial.println('|');
  delay(1000);
}

void loop() {
  if (Serial.available()) {
    char c = (char)Serial.read();
    //Serial.println((int)c);
    switch (c) {
      case '0': // Accendi/Spegni
        power();
        break;

      case '1': // Verifica connessione
        verificaConnessione();
        break;

      case '2': // Imposta debug verboso
        debugVerboso();
        break;

      case '$': // Controlla stato PIN
        controllaPIN();
        break;

      case '%': // Configua PIN
        configuraPIN();
        break;

      case '3': // Stampa qualità GSM
        stampaQualitaGSM();
        break;

      case '4': // Invia SMS
        inviaSMS(DESTINATARIO, "Ciao mondo");
        break;

      case '5': // Leggi SMS
        letturaSMS();
        break;

      case '/': // Stampa SMS
        stampaSMS();
        break;

      case '(': // Rimuovi SMS
        rimuoviSMS();
        break;

      case '6': // Chiamata
        chiamata(DESTINATARIO);
        break;

      case '7': // Avvia GPS
        avviaGPS();
        break;

      case '=': // Stampa qualità GPS
        stampaQualitaGPS();
        break;

      case '8': // Invia posizione GPS
        inviaPosizione(DESTINATARIO);
        break;

      case '?': // Stampa posizione GPS
        stampaGPS();
        break;

      case '9': // Spegni GPS
        spegniGPS();
        break;

      case '\r':
      case '\n':
        Serial.println('|');
        break;

      default:
        Serial.print("| Comando sconosciuto: ");
        stampaChar(c);
    }
  }

  stampaRisposte();
}

// Leggi quello che manda la shield e stampalo nella seriale del PC
void stampaRisposte() {
  while (GSMSerial.available())
    Serial.print((char)GSMSerial.read());
}

// Invia un carattere nella seriale del PC
void stampaChar(char c) {
  Serial.print(c);
  Serial.print(" (ASCII ");
  Serial.print((int)c);
  Serial.println(')');
}

// Invia un carattere nella seriale della shield
void inviaChar(char payload) {
  Serial.print("| INVIO CHAR: ");
  stampaChar(payload);
  GSMSerial.print(payload);
}

// Invia una stringa nella seriale della shield
void inviaStringa(const char* payload) {
  Serial.print("| INVIO STRINGA: ");
  Serial.print(payload);
  Serial.println("<CR><NL>");
  GSMSerial.println(payload);
}

// Invia una stringa nella seriale della shield, leggi la risposta e inviala nella seriale del PC
void inviaStringaStampaRisposte(const char* payload) {
  inviaStringa(payload);
  delay(100);
  stampaRisposte();
  Serial.println();
}

// Consuma i caratteri di fine linea dalla seriale del PC
void consumaFineLinea() {
  if (Serial.available()) {
    int c = Serial.peek();
    if (c == '\r' || c == '\n')
      Serial.read();
  }
}

// Accendi o spegni la shield
void power()
{
  Serial.println("| Accensione/Spegnimento...");
  digitalWrite(POWER_PIN, LOW);
  delay(1000);
  digitalWrite(POWER_PIN, HIGH);
  delay(4000);
  Serial.println("| Acceso/Spento...");
}

// Verifica se la connessione fra Arduino e shield funziona
void verificaConnessione() {
  Serial.println("| Verifica della connessione con il comando 'AT', dovrebbe rispondere 'OK'...");
  inviaStringaStampaRisposte("AT");
}

// Indica alla shield di rispondere verbosamente ai comandi
void debugVerboso() {
  Serial.println("| Impostazione debug veroboso...");
  inviaStringaStampaRisposte("AT+CMEE=2");
  //inviaStringaStampaRisposte("AT+CMEE?");
}

// Verifica se il pin è inserito, non inserito o non necessario
void controllaPIN() {
  Serial.println("| Coontrollo stato PIN...");
  inviaStringaStampaRisposte("AP+CPIN?");
}

// Inserisci il pin
void configuraPIN() {
  Serial.println("| Configuarzione PIN...");
#define CPIN_DIM 15
  char cpin[CPIN_DIM];
  snprintf(cpin, CPIN_DIM, "AP+CPIN=%d", PIN_SIM); // es: AP+CPIN=7975
  inviaStringaStampaRisposte(cpin);
  controllaPIN();
}

// Invia un SMS con la shield
void inviaSMS(const char* numero, const char* payload) {
  // Protocollo:
  // AT+CMGF=1 (abilita la modalità testuale)
  // AT+CSCA=<NUMERO> (opzionale, <NUMERO> è il SMSC dell'operatore)
  // AT+CMGS="<NUMERO>"<TESTO><EOF> (<NUMERO> è il numero del destinatario, <EOF> è End Of File, carattere ASCII 0x1A)

  Serial.println("| Invio SMS...");

  inviaStringaStampaRisposte("AT+CMGF=1");

#define CMGS_DIM 30
  char cmgs[CMGS_DIM];
  snprintf(cmgs, CMGS_DIM, "AT+CMGS=\"%s\"", numero); // es: AT+CMGS="+391234567890"
  inviaStringaStampaRisposte(cmgs);

  inviaStringaStampaRisposte(payload);

  inviaChar(0x1A); // EOF
}

// Stampa gli SMS
void stampaSMS() {
  // https://www.developershome.com/sms/readSmsByAtCommands.asp
  // https://www.developershome.com/sms/cmgrCommand.asp
  // Protocollo:
  // AT+CMGF=1
  // e poi...
  // AT+CNMI=1,2,0,0,0
  // ...per leggerli automaticamente in push, oppure...
  // AT+CMGL="REC UNREAD"
  // AT+CMGR=3

  inviaStringaStampaRisposte("AT+CMGF=1");
  inviaStringaStampaRisposte("AT+CMGL=\"REC UNREAD\"");
}

void letturaSMS() {
#define MAX_MESSAGGI 10
  struct sms messaggi[MAX_MESSAGGI];
  unsigned int num = leggiTuttiSMS(messaggi, MAX_MESSAGGI);
  if (num == 0) {
    Serial.println("| Nessun nuovo messaggio");
  } else {
    for (int  i = 0; i < num; i++) {
      Serial.print("| ");
      Serial.print(messaggi[i].numero);
      Serial.print(" ha scritto: ");
      Serial.println(messaggi[i].payload);
    }
  }
}

// Attendi una stringa che inizi con il contenuto di "ricerca" (ritorna true)
// Esci se arriva una strigna uguale al contenuto di "stopper" (ritorna false)
boolean cercaStringaDaRisposta(const char* ricerca, const char *stopper, char* output, unsigned int maxDim) {
  String s;
  char* c_str;
  do {
    s = GSMSerial.readStringUntil('\n');
    if (s.equals("\0"))
      return false;
    s.trim();
    Serial.println(s);
    if (stopper != NULL && s.equals(stopper))
      return false;
  } while (!s.startsWith(ricerca));
  snprintf(output, maxDim, s.c_str());
  return true;
}

// Leggi gli SMS
// Ritorna il numero di messaggi letti (<= maxMessaggi)
unsigned int leggiTuttiSMS(struct sms *messaggi, unsigned int maxMessaggi) {
  inviaStringaStampaRisposte("AT+CMGF=1");
  inviaStringa("AT+CMGL=\"REC UNREAD\"");
  delay(100);

  unsigned int i = 0;
  boolean lineaValida = false;
#define CMGL_DIM 100
  char cmgl[CMGL_DIM];

  do {
    Serial.println("| Ricerca messaggio...");
    lineaValida = cercaStringaDaRisposta("+CMGL:", "OK", cmgl, CMGL_DIM);

    // Mock
    //cmgl="+CMGL: 10,\"REC UNREAD\",\"+393471840366\",\"\",\"19/04/06,11:12:13+08\""
    //lineaValida=true;

    if (lineaValida) {
      messaggi[i] = leggiSMS(cmgl);
      i++;
    }

  } while (lineaValida && i < maxMessaggi);
  Serial.print("| Completato lettura ");
  Serial.print(i);
  Serial.println(" messaggi");
  return i;

  // Mock
  //sprintf(messaggi[0].numero, DESTINATARIO);
  //sprintf(messaggi[0].payload, "AaBbCc");
  //sprintf(messaggi[1].numero, DESTINATARIO);
  //sprintf(messaggi[1].payload, "DdEeFf");
  //return 2;
}

struct sms leggiSMS(const char *rigaCMGL) {
  struct sms messaggio;
  int id;
  char stato[11];
  
  Serial.println("| Lettura numero messaggio...");
  int n = sscanf(rigaCMGL, "+CMGL: %d,%[^,],\"%[^\"]\",%*", &id, stato, messaggio.numero);
  Serial.println("| Lettura numero messaggio terminata");
  
  if (n != 3) {
    Serial.println("| Lettura numero fallita");
    snprintf(messaggio.numero, SMS_NUMERO_DIM, "NUMERO SCONOSCIUTO");
  }
  //Serial.println(stato);
  //Serial.println(messaggi[i].numero);

  Serial.println("| Lettura contenuto messaggio...");
  unsigned int j = 0;
  do {
    messaggio.payload[j] = (char)GSMSerial.read();
    stampaChar(messaggio.payload[j]);
    j++;
  } while (
    j < SMS_PAYLOAD_DIM - 1 && (
      j < 3 || (
        messaggio.payload[j - 3] != '\n' &&
        messaggio.payload[j - 2] != '\r' &&
        messaggio.payload[j - 1] != '\n' &&
        messaggio.payload[j] != '\r' &&
        messaggio.payload[j] != '\0'
      )
    )
  );

  messaggio.payload[j] = '\0';
  Serial.println("| Lettura contenuto messaggio terminata");

  return messaggio;
}

void rimuoviSMS(){
  Serial.println("| Rimozione SMS...");
  inviaStringa("AT+CMGD=0,3");
  Serial.println("| SMS rimossi");
}

// Chiamata vocale, usando il jack audio della shield
void chiamata(const char* numero) {
  // Protocollo:
  // ATD<NUMERO> (<NUMERO> è il numero del destinatario)

#define ATD_DIM 20
  char atd[ATD_DIM];
  snprintf(atd, ATD_DIM, "ATD%s;", numero); // es: ATD+391234567890
  inviaStringaStampaRisposte(atd);
}

// Avvia il GPS
void avviaGPS() {
  Serial.println("| Avvio GPS...");
  inviaStringaStampaRisposte("AT+CGPSPWR=1");
}

// Spegni il GPS
void spegniGPS() {
  Serial.println("| Spegnimento GPS...");
  inviaStringaStampaRisposte("AT+CGPSPWR=0");
}

// Verifica la qualità della connessione alla rete GSM
void stampaQualitaGSM() {
  Serial.println("| Controllo qualità GSM...");
  inviaStringaStampaRisposte("AT+CSQ");
}

// Verifica se il GPS ha ottenuto la geolocalizzazione e la precisione
void stampaQualitaGPS() {
  Serial.println("| Controllo qualità GPS...");
  inviaStringaStampaRisposte("AT+CGPSSTATUS?");
}

// Stampa la posizione ottenuta dal GPS
void stampaGPS() {
  Serial.println("| Stampa posizione GPS...");
  inviaStringaStampaRisposte("AT+CGPSINF=0");
}

// Ottieni la posizione come struct posizione
// return successo?
boolean getPosizione(struct posizione* p) {
  Serial.println("| Lettura posizione...");
  inviaStringa("AT+CGPSINF=0");
  delay(100);

#define CGPSINF_DIM 100
  char cgpsinf[CGPSINF_DIM];
  cercaStringaDaRisposta("+CGPSINF:", "OK", cgpsinf, CGPSINF_DIM);

  // Mock
  //res = "+CGPSINF: 0,2234.931817,11357.122485, 92.461185,20141031041141.000, 88,12,0.000000,0.000000"; // test, riumuovere quando funzionerà la board

  // https://cdn-shop.adafruit.com/datasheets/SIM808_GPS_Application_Note_V1.00.pdf#page=8
  // https://m2msupport.net/m2msupport/atcgpsinf-get-current-gps-location-info/
  // http://wiki.seeedstudio.com/Mini_GSM_GPRS_GPS_Breakout_SIM808/#get-location-with-gps
  char lat[12], lon[12];
  boolean ok = 2 == sscanf(cgpsinf, "+CGPSINF: %*d,%11[^,],%11[^,],%*s", &lat, &lon);
  //int laa, lab, lac, loa, lob, loc;
  //boolean ok = 2 == sscanf(cgpsinf, "+CGPSINF: %*d,%2d%2d.%6d,%2d%2d.%6d,%*s", &laa, &lab, &lac, &loa, &lob, &loc);
  if (ok) {
    //Serial.println(lat);
    //Serial.println(lon);

    // TODO : Correggere conversione, https://www.dfrobot.com/forum/viewtopic.php?p=7708&sid=1bc353ede2c4190a6636a48bbdd11a38#p7708
    p->latitudine = atof(lat) / 100;
    p->longitudine = atof(lon) / 100;
    Serial.println("| Posizione letta");
  } else {
    p->latitudine = 0;
    p->longitudine = 0;
  }
  return ok;
}

// Invia via SMS la posizione GPS
void inviaPosizione(const char* numero) {
  struct posizione p;
  Serial.println("| Invio posizione...");
#define STR_DIM 140
  char lat[12], lon[12], str[STR_DIM];
  if (getPosizione(&p)) {
    dtostrf(p.latitudine, 11, 8, lat);
    dtostrf(p.longitudine, 11, 8, lon);
    //snprintf(str, STR_DIM, "LAT %s - LON %s", lat, lon);
    snprintf(str, STR_DIM, "Latitudine: %s\rLongitudine: %s\rURL: https://www.google.com/maps/search/?api=1&query=%s,%s", lat, lon, lat, lon);
    inviaSMS(numero, str);
  }
}
