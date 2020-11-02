#define PIN A1
#define SERIAL_SPEED 57600

void setup() {
  
  pinMode(PIN, INPUT);
  Serial.begin(SERIAL_SPEED);

}

void loop() {
  uint16_t value = analogRead(PIN) >> 2;
  Serial.println(value);
  
}
