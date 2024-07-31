#ifndef LED_H
#define LED_H

#define LED_IO 9 // PB1
#define LED_SPEED (F_CPU / 8000000UL) * 2048

// LED states
enum { LED_OFF,
       LED_ON,
       LED_BLINK };

void led_setup(void);
void led_enable(uint8_t state);
void led_disable(void);
void led_loop(void);

#endif