#ifndef BATT_H
#define BATT_H

#define BATT_MIN 2000 // Minimum battery voltage

// Functions
void battery_setup(void);
uint16_t battery_getvoltage(void);

#endif