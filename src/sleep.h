#ifndef SLEEP_H
#define SLEEP_H

#define SLEEP_MIN_INTERVAL 60

// Functions
void sleep_set(uint16_t seconds);
uint16_t sleep_get(void);
void sleep_interval();

#endif