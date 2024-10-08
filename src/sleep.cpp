#include <Arduino.h>
#include <LowPower.h>
#include "sleep.h"
#include "button.h"

static uint16_t sleep_seconds;
static uint8_t sleep_seconds_int = 0;

// Low power sleep mode for a defined time
void sleep_interval(void) {
  uint16_t seconds = sleep_seconds;
  if(seconds < SLEEP_MIN_INTERVAL) {
    seconds = SLEEP_MIN_INTERVAL;
  }
  // Sleep given duty cycle
  do {
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    // Check if sleep should be interrupted, but enforce SLEEP_MIN_INTERVAL
    if(button_interrupt() && sleep_seconds_int == 0) {
      sleep_seconds_int = SLEEP_MIN_INTERVAL;
      seconds = 1;
    }
    if(sleep_seconds_int > 0) {
      --sleep_seconds_int;
    }
  } while(--seconds > 0);
}

// Low power sleep mode forever
void sleep_forever(void) {
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

// Set sleep mode interval
void sleep_set(uint16_t seconds) {
  sleep_seconds = seconds;
}

// Get sleep mode interval
uint16_t sleep_get(void) {
  return sleep_seconds;
}