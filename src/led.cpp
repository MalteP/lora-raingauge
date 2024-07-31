#include <Arduino.h>
#include "led.h"

static uint8_t led_state = 0;
static uint16_t led_counter = 0;

// Initialize LED IO
void led_setup(void) {
  pinMode(LED_IO, OUTPUT);
  digitalWrite(LED_IO, HIGH);
}

// Enable LED
void led_enable(uint8_t state) {
  led_state = state;
  if(led_state == LED_ON) {
    digitalWrite(LED_IO, LOW);
  }
}

// Disable LED
void led_disable(void) {
  led_state = LED_OFF;
  digitalWrite(LED_IO, HIGH);
}

// Must be called inside the main loop, used for LED blinking
void led_loop(void) {
  if(led_state == LED_BLINK) {
    if(++led_counter > LED_SPEED) {
      digitalWrite(LED_IO, !digitalRead(LED_IO));
      led_counter = 0;
    }
  }
}