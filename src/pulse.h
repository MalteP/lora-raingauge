#ifndef PULSE_H
#define PULSE_H

#define PULSE_PIN 5       // D5 (PD5) as pulse input
#define PULSE_INT PCINT21 // Pin change interrupt of PD5

// Functions
void pulse_setup(void);
void pulse_set(uint16_t value);
uint16_t pulse_get(void);

#endif