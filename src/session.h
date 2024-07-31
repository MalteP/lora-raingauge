#ifndef SESSION_H
#define SESSION_H

#define BUTTON_IO 15 // D15 (PC1) Reset session when low

#define SESSION_FRAM_LMIC_OFFSET 0
#define SESSION_FRAM_USER_OFFSET 1024

void session_setup(void);
void session_changed(bool save_now);
void session_loop(void);
void session_resetduty(void);

#endif