// https://www.elecrow.com/wiki/index.php?title=SIM808_GPRS/GSM%2BGPS_Shield_v1.1
// http://wiki.seeedstudio.com/Mini_GSM_GPRS_GPS_Breakout_SIM808/#set-baud-and-enable-charging-function
// https://learn.adafruit.com/adafruit-fona-808-cellular-plus-gps-breakout/overview
// https://m2msupport.net/m2msupport/sms-at-commands/

#include <stdio.h>

#define BAUD_RATE 9600

#define POWER_PIN 9
#define GSMSerial Serial3

#define PIN_SIM 7975
#define SMSC "AT+CSCA=?" // Lettura SMSC attuale
//#define SMSC "AT+CSCA=\"+393770001006\"" // SMSC PosteMobile

#define DESTINATARIO "+393471840366"

struct posizione {
  double latitudine, longitudine;
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
  Serial.println("| 5 --> Esegui chiamata");
  Serial.println("| 6 --> Avvia GPS");
  Serial.println("| 7 --> Stampa qualità GPS");
  Serial.println("| 8 --> Stampa posizione GPS");
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

      case '5': // Chiamata
        chiamata(DESTINATARIO);
        break;

      case '6': // Avvia GPS
        avviaGPS();
        break;

      case '7': // Stampa qualità GPS
      case '8': // Stampa posizione GPS
        Serial.println("Non implementato");
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

void stampaRisposte() {
  while (GSMSerial.available())
    Serial.print((char)GSMSerial.read());
}

void stampaChar(char c) {
  Serial.print(c);
  Serial.print(" (ASCII ");
  Serial.print((int)c);
  Serial.println(')');
}

void power()
{
  Serial.println("| Accensione/Spegnimento...");
  digitalWrite(POWER_PIN, LOW);
  delay(1000);
  digitalWrite(POWER_PIN, HIGH);
  delay(4000);
  Serial.println("| Acceso/Spento...");
}

void verificaConnessione() {
  Serial.println("| Verifica della connessione con il comando 'AT', dovrebbe rispondere 'OK'...");
  inviaStringaStampaRisposte("AT");
}

void debugVerboso() {
  Serial.println("| Impostazione debug veroboso...");
  inviaStringaStampaRisposte("AT+CMEE=2");
  //inviaStringaStampaRisposte("AT+CMEE?");
}

void controllaPIN() {
  Serial.println("| Coontrollo stato PIN...");
  inviaStringaStampaRisposte("AP+CPIN?");
}

void configuraPIN() {
  Serial.println("| Configuarzione PIN...");
  char cpin[15];
  snprintf(cpin, 15, "AP+CPIN=%d", PIN_SIM); // AP+CPIN=7975
  inviaStringaStampaRisposte(cpin);
  controllaPIN();
}

void powerOff() {
  Serial.println("| Spegnimento...");
  power();
  delay(2000);
  Serial.println("| Spento");
}

void inviaChar(char payload) {
  Serial.print("| INVIO CHAR: ");
  stampaChar(payload);
  GSMSerial.print(payload);
}

void inviaStringa(const char* payload) {
  Serial.print("| INVIO STRINGA: ");
  Serial.print(payload);
  Serial.println("<CR><NL>");
  GSMSerial.println(payload);
}

void inviaStringaStampaRisposte(const char* payload) {
  inviaStringa(payload);
  delay(100);
  stampaRisposte();
  Serial.println();
}

void consumaFineLinea() {
  if (Serial.available()) {
    int c = Serial.peek();
    if (c == '\r' || c == '\n')
      Serial.read();
  }
}

void inviaSMS(const char* numero, const char* payload) {
  Serial.println("| Invio SMS...");

  inviaStringaStampaRisposte("AT+CMGF=1");    //Because we want to send the SMS in text mode

  inviaStringaStampaRisposte(SMSC);

  char cmgs[30];
  snprintf(cmgs, 30, "AT+CMGS=\"%s\"", numero); // AT+CMGS="+393471840366"
  inviaStringaStampaRisposte(cmgs);

  inviaStringaStampaRisposte(payload);   //The text for the message

  inviaChar(0x1A);  //Equivalent to sending Ctrl+Z
}

void chiamata(const char* numero) {
  char atd[20];
  snprintf(atd, 20, "ATD%s;", numero); // ATD+393471840366
  inviaStringaStampaRisposte(atd);
}

void avviaGPS() {
  Serial.println("| Avvio GPS...");
  inviaStringaStampaRisposte("AT+CGPSPWR=1");
}

void spegniGPS() {
  Serial.println("| Spegnimento GPS...");
  inviaStringaStampaRisposte("AT+CGPSPWR=0");
}

void stampaQualitaGSM() {
  Serial.println("| Controllo qualità GSM...");
  inviaStringaStampaRisposte("AT+CSQ");
}

void stampaQualitaGPS() {
  Serial.println("| Controllo qualità GPS...");
  inviaStringaStampaRisposte("AT+CGPSSTATUS?");
}

void stampaGPS() {
  Serial.println("| Stampa posizione GPS...");
  inviaStringaStampaRisposte("AT+CGPSINF=0");
}

struct posizione getPosizione() {
  inviaStringa("AT+CGPSINF=0");
  
  const char* res;
  String s;
  //do {
  //  s = GSMSerial.readStringUntil('\r');
  //} while (s.compareTo("AT+CGPSINF=0") != 0);
  //res = GSMSerial.readStringUntil('\r').c_str();
  res = "+CGPSINF: 0,2234.931817,11357.122485, 92.461185,20141031041141.000, 88,12,0.000000,0.000000"; // test, riumuovere quando funzionerà la board

  struct posizione p;
  // https://cdn-shop.adafruit.com/datasheets/SIM808_GPS_Application_Note_V1.00.pdf#page=8
  // https://m2msupport.net/m2msupport/atcgpsinf-get-current-gps-location-info/
  // http://wiki.seeedstudio.com/Mini_GSM_GPRS_GPS_Breakout_SIM808/#get-location-with-gps
  sscanf(res, "+CGPSINF: %f,%f,%n",&(p.latitudine),&(p.longitudine));

  return p;
}
