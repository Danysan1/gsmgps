// https://www.elecrow.com/wiki/index.php?title=SIM808_GPRS/GSM%2BGPS_Shield_v1.1
// http://wiki.seeedstudio.com/Mini_GSM_GPRS_GPS_Breakout_SIM808/#set-baud-and-enable-charging-function
// https://learn.adafruit.com/adafruit-fona-808-cellular-plus-gps-breakout/overview

#include <stdio.h>

#define PC_BAUD_RATE 9600
#define SIM_BAUD_RATE 9600
#define POWER_PIN 9
#define GSMSerial Serial3
#define SMSC "AT+CSCA=?"
//#define SMSC "AT+CSCA=\"+393770001006\"" // PosteMobile
#define NUMERO "+393471840366"
//#define NUMERO "+393470974284"


void setup() {
  Serial.begin(PC_BAUD_RATE);
  pinMode(POWER_PIN, OUTPUT);
  Serial.println("| Comandi:");
  Serial.println("| 0 --> Spegni");
  Serial.println("| 1 --> Accendi");
  Serial.println("| 2 --> Check");
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
      case '0': // Spegni
        powerOff();
        break;

      case '1': // Accendi
        powerOn();
        break;

      case '2': // Check
        inviaStringa("AT");
        break;

      case '3': // Stampa qualità GSM
        stampaQualitaGSM();
        break;

      case '4': // Invia SMS
        inviaSMS(NUMERO, "Ciao mondo");
        break;

      case '5': // Chiamata
        chiamata(NUMERO);
        break;

      case '6': // Avvia GPS
        avviaGPS();
        break;

      case '7': // Stampa qualità GPS
      case '8': // Stampa posizione GPS
        Serial.println("Non implementato");
        break;

      case '9':
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

  leggiRisposte();
}

void leggiRisposte() {
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
  digitalWrite(POWER_PIN, LOW);
  delay(1000);               // wait for 1 second
  digitalWrite(POWER_PIN, HIGH);
}

void powerOn() {
  Serial.println("| Accensione...");
  power();
  Serial.println("| Acceso");

  Serial.println("| Connessione...");
  GSMSerial.begin(SIM_BAUD_RATE); // the GPRS/GSM baud rate
  delay(4000);
  Serial.println("| Connesso (verificare se la connessione funziona con il comando 'AT', dovrebbe rispondere 'OK')");

  //inviaStringa("AP+CPIN?");
  //inviaStringa("AP+CPIN=7975");
  //inviaStringa("AP+CPIN?");
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
  Serial.println("<CR>");
  GSMSerial.println(payload);
  delay(100);
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

  inviaStringa("AT+CMGF=1");    //Because we want to send the SMS in text mode
  leggiRisposte();

  inviaStringa(SMSC);
  leggiRisposte();

  char cmgs[30];
  snprintf(cmgs, 30, "AT+CMGS=\"%s\"", numero); // AT+CMGS="+393471840366"
  inviaStringa(cmgs);
  leggiRisposte();

  inviaStringa(payload);   //The text for the message
  leggiRisposte();
  Serial.println();

  inviaChar(0x1A);  //Equivalent to sending Ctrl+Z
}

void chiamata(const char* numero) {
  char atd[20];
  snprintf(atd, 20, "ATD+%s;", numero);
  GSMSerial.println("ATD+393471840366;");
}

void avviaGPS() {
  Serial.println("| Avvio GPS...");
  //inviaStringa("AT+CGPSPWR=1");
  GSMSerial.print("AT+CGPSPWR=1\r");
}

void spegniGPS() {
  Serial.println("| Spegnimento GPS...");
  inviaStringa("AT+CGPSPWR=0");
}

void stampaQualitaGSM() {
  Serial.println("| Controllo qualità GSM...");
  inviaStringa("AT+CSQ");
}

void stampaQualitaGPS() {
  Serial.println("| Controllo qualità GPS...");
  inviaStringa("AT+CGPSSTATUS?");
}
