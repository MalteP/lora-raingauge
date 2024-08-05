#include <Arduino.h>
#include <LowPower.h>
#include "sleep.h"

static uint16_t sleep_seconds;

// Low power sleep mode for a defined time
void sleep_interval(void) {
  uint16_t seconds = sleep_seconds;
  if(seconds < SLEEP_MIN_INTERVAL) {
    seconds = SLEEP_MIN_INTERVAL;
  }
  // Sleep given duty cycle
  do {
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  } while(--seconds > 0);
}

// Set sleep mode interval
void sleep_set(uint16_t seconds) {
  sleep_seconds = seconds;
}

// Get sleep mode interval
uint16_t sleep_get(void) {
  return sleep_seconds;
}