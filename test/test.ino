// https://www.elecrow.com/wiki/index.php?title=SIM808_GPRS/GSM%2BGPS_Shield_v1.1
// https://learn.adafruit.com/adafruit-fona-808-cellular-plus-gps-breakout/overview

#include <SoftwareSerial.h>
#include <string.h>

#define POWER_PIN 9
//#define TX_PIN 14
//#define RX_PIN 15
//SoftwareSerial GSMSerial(RX_PIN, TX_PIN);
#define GSMSerial Serial3


void setup() {
  Serial.begin(19200);
  pinMode(POWER_PIN, OUTPUT);
  //pinMode(TX_PIN, OUTPUT);
  //pinMode(RX_PIN, INPUT);
  Serial.println("| Comandi:");
  Serial.println("| 1 --> Spegni");
  Serial.println("| 2 --> Accendi");
  Serial.println("| 3 --> Check");
  Serial.println("| 4 --> Invia SMS");
  Serial.println("| 5 --> Stampa posizione GPS");
  Serial.println("| 6 --> Esegui chiamata");
  Serial.println('|');
  delay(1000);
}

void loop() {
  if (Serial.available()) {
    char c = (char)Serial.read();
    //Serial.println((int)c);
    switch (c) {
      case '1':
        powerOff();
        break;

      case '2':
        powerOn();
        break;

      case '3':
        inviaStringa("AT");
        break;

      case '4':
        inviaSMS("Ciao mondo");
        break;

      case '5':
      case '6':
        Serial.println("Non implementato");
        break;

      case '\r':
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
  GSMSerial.begin(19200); // the GPRS/GSM baud rate
  delay(4000);
  Serial.println("| Connesso (verificare se la connessione funziona con il comando 'AT\r', dovrebbe rispondere 'OK')");
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
  if (Serial.available() && Serial.peek() == '\r') {
    Serial.read();
  }
}

void inviaSMS(const char* payload) {
  Serial.println("| Invio SMS...");

  inviaStringa("AT+CMGF=1");    //Because we want to send the SMS in text mode
  leggiRisposte();

  inviaStringa("AT+CMGS=\"+393471840366\"");    //Start accepting the text for the message
  leggiRisposte();

  inviaStringa(payload);   //The text for the message
  leggiRisposte();
  Serial.println();

  inviaChar(0x1A);  //Equivalent to sending Ctrl+Z
}
