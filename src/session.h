#ifndef SESSION_H
#define SESSION_H

#define BUTTON_IO 15 // D15 (PC1) Reset session when low

#define SESSION_FRAM_LMIC_OFFSET 0
#define SESSION_FRAM_USER_OFFSET 1024

void session_setup(void);
void session_changed(bool save_now);
void session_check_changes(void);
void session_defaults(void);
void session_resetduty(void);
void session_sleep(void);
void session_wakeup(void);

// Userdata stored in FRAM
struct session_userdata_t {
  uint16_t pulsecounter;
  uint16_t sleepinterval;
};

#endif