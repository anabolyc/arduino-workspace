#define PIN_SENS 1
#define PIN_LED  1

#define TRESHOLD 75

void setup() {

  pinMode(PIN_SENS, INPUT);
  pinMode(PIN_LED, OUTPUT);

}

void loop() {
  uint8_t value = analogRead(PIN_SENS);
  uint8_t led_state = (value > TRESHOLD) ? LOW : HIGH;
  digitalWrite(PIN_LED, led_state);
  delay(100);
  //blink_times(value);
  //delay(2000);
}

#define BLINK_TIME 100

void blink_once() {
  digitalWrite(PIN_LED, HIGH);
  delay(BLINK_TIME);
  digitalWrite(PIN_LED, LOW);
  delay(BLINK_TIME);
}

void blink_times(uint8_t times) {
  for (uint8_t i = 0; i < times; i++) {
    blink_once();
  }
}


