#include <Arduino.h>
#include "button.h"

static bool button_status;

void button_setup(void) {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  PCICR |= (1 << PCIE1);       // Enable PCINT
  PCMSK1 |= (1 << BUTTON_INT); // Configure PCINT mask
  button_status = 0;
}

bool button_read(void) {
  return digitalRead(BUTTON_PIN) ? 0 : 1;
}

bool button_interrupt(void) {
  bool status = button_status;
  button_status = 0;
  return status;
}

// ISR for PCINT
ISR(PCINT1_vect) {
  // Rising edge
  if(digitalRead(BUTTON_PIN)) {
    button_status = 1;
  }
}