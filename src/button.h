#ifndef BUTTON_H
#define BUTTON_H

#define BUTTON_PIN 15     // D15 (PC1) Active low
#define BUTTON_INT PCINT9 // Pin change interrupt of PC1

void button_setup(void);
bool button_read(void);
bool button_interrupt(void);

#endif