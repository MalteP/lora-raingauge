#include <Arduino.h>
#include <LowPower.h>
#include "sleep.h"

// Low power sleep mode for a defined time
void sleep_interval(uint32_t seconds) {
  // Sleep given duty cycle
  do {
    LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
  } while(--seconds > 0);
}