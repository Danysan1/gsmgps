// https://forum.arduino.cc/index.php?topic=515839.0

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

