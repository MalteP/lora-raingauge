#include <Arduino.h>
#include <lmic.h>
#include "config.h"
#include "pulse.h"

static uint16_t pulse_ctr;

// Initialize interrupt based pulse counter
void pulse_setup(void) {
#ifndef USE_EXTERNAL_PULLUP
  pinMode(PULSE_PIN, INPUT_PULLUP); // Configure pulse input
#else
  pinMode(PULSE_PIN, INPUT);
#endif
  PCICR |= (1 << PCIE2);      // Enable PCINT
  PCMSK2 |= (1 << PULSE_INT); // Configure PCINT mask
  pulse_ctr = 0;
}

// Set pulse counter value
void pulse_set(uint16_t value) {
  pulse_ctr = value;
}

// Get pulse counter value
uint16_t pulse_get(void) {
  return pulse_ctr;
}

// ISR for PCINT
ISR(PCINT2_vect) {
  // Falling edge
  if(!digitalRead(PULSE_PIN)) {
    ++pulse_ctr;
  }
}