// https://www.elecrow.com/wiki/index.php?title=SIM808_GPRS/GSM%2BGPS_Shield_v1.1
// http://wiki.seeedstudio.com/Mini_GSM_GPRS_GPS_Breakout_SIM808/#set-baud-and-enable-charging-function
// https://learn.adafruit.com/adafruit-fona-808-cellular-plus-gps-breakout/overview

#include <stdio.h>

#define PC_BAUD_RATE 9600
#define SIM_BAUD_RATE 9600

#define POWER_PIN 9
#define GSMSerial Serial3

#define SMSC "AT+CSCA=?" // Lettura SMSC attuale
//#define SMSC "AT+CSCA=\"+393770001006\"" // SMSC PosteMobile

#define NUMERO "+393471840366"
//#define NUMERO "+393470974284"

void setup() {
  Serial.begin(PC_BAUD_RATE);
  pinMode(POWER_PIN, OUTPUT);
  Serial.println("| Comandi:");
  Serial.println("| 0 --> Accendi/Spegni");
  Serial.println("| 1 --> Connetti");
  Serial.println("| 2 --> Check");
  Serial.println("| $ --> Debug verboso");
  Serial.println("| % --> Controlla stato PIN");
  Serial.println("| & --> Configura PIN");
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

      case '1': // Connetti
        connetti();
        break;

      case '2': // Check
        verificaConnessione();
        break;

      case '$': // Debug verboso
        debugVerboso();
        break;

      case '%': // Controlla stato PIN
        controllaPIN();
        break;

      case '&': // Configua PIN
        configuraPIN();
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
  Serial.println("| Accensione/Spegnimento...");
  digitalWrite(POWER_PIN, LOW);
  delay(1000);               // wait for 1 second
  digitalWrite(POWER_PIN, HIGH);
  Serial.println("| Acceso/Spento...");
}

void connetti() {
  Serial.println("| Connessione...");
  GSMSerial.begin(SIM_BAUD_RATE); // the GPRS/GSM baud rate
  delay(4000);
  Serial.println("| Connesso");
}

void verificaConnessione() {
  Serial.println("| Verifica della connessione con il comando 'AT', dovrebbe rispondere 'OK'...");
  inviaStringaLeggiRisposta("AT");
}

void debugVerboso() {
  Serial.println("| Impostazione debug veroboso...");
  inviaStringaLeggiRisposta("AT+CMEE=2");
  inviaStringaLeggiRisposta("AT+CMEE?");
}

void controllaPIN(){
  Serial.println("| Coontrollo stato PIN...");
  inviaStringaLeggiRisposta("AP+CPIN?\n\r");
  }

void configuraPIN() {
  Serial.println("| Configuarzione PIN...");
  inviaStringaLeggiRisposta("AP+CPIN=7975");
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
  Serial.println("<CR>");
  GSMSerial.println(payload);
}

void inviaStringaLeggiRisposta(const char* payload) {
  inviaStringa(payload);
  delay(100);
  leggiRisposte();
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

  inviaStringaLeggiRisposta("AT+CMGF=1");    //Because we want to send the SMS in text mode

  inviaStringaLeggiRisposta(SMSC);

  char cmgs[30];
  snprintf(cmgs, 30, "AT+CMGS=\"%s\"", numero); // AT+CMGS="+393471840366"
  inviaStringaLeggiRisposta(cmgs);

  inviaStringaLeggiRisposta(payload);   //The text for the message

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
  inviaStringaLeggiRisposta("AT+CGPSPWR=0");
}

void stampaQualitaGSM() {
  Serial.println("| Controllo qualità GSM...");
  inviaStringaLeggiRisposta("AT+CSQ");
}

void stampaQualitaGPS() {
  Serial.println("| Controllo qualità GPS...");
  inviaStringaLeggiRisposta("AT+CGPSSTATUS?");
}
