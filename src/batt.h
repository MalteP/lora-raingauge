#ifndef BATT_H
#define BATT_H

#define BATT_MIN 2000      // Minimum battery voltage during operation
#define BATT_MIN_BOOT 2100 // Minimum battery voltage at startup

// Functions
void battery_setup(void);
uint16_t battery_getvoltage(void);

#endif