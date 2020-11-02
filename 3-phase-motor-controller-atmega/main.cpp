#include <avr/io.h>
#include <util/delay.h>
#include "main.h"

#define LED_PIN 5

void setup(void)
{
  DDRB |= 1 << LED_PIN;
}

void loop(void)
{
  PORTB |= 1 << LED_PIN;
  _delay_ms(100);
  PORTB &= ~(1 << LED_PIN);
  _delay_ms(100);
}
