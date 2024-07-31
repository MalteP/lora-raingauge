#include <avr/io.h>
#include <util/delay.h>
#include "batt.h"

// Initialize reference and ADC
void battery_setup(void) {
  ADMUX = (1 << REFS0);  // AVCC as reference
  ADCSRA |= (1 << ADSC); // Dummy read
  while(ADCSRA & (1 << ADSC)) {
    // Wait until ADC finished
  };
}

// With AVCC as reference we measure the internal
// 1,1V bandgap reference to calculate the battery voltage
uint16_t battery_getvoltage(void) {
  ADMUX |= (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0); // Measure VBG
  _delay_us(500);                                                 // Let the reference voltage stabilize
  ADCSRA |= (1 << ADSC);                                          // Start conversion
  while(ADCSRA & (1 << ADSC)) {
    // Wait until ADC finished
  };
  return (1100UL * 1023 / ADC);
}