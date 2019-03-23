// https://forum.arduino.cc/index.php?topic=515839.0
// https://www.micromedia-int.com/en/gsm-2/73-gsm/668-at-cpin-answers-with-error

#define BAUD_RATE 9600
#define SerialGSM Serial3

void setup() {
  Serial.begin(BAUD_RATE);
  SerialGSM.begin(BAUD_RATE);
  Serial.println("Pronto");
}

void loop() {
  if (Serial.available()) {
    SerialGSM.write(Serial.read());
  }

  if (SerialGSM.available()) {
    Serial.write(SerialGSM.read());
  }
}

