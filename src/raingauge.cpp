#include <Arduino.h>
#include <lmic.h>
#include <hal/hal.h>
#include <TinyBME280.h>
#include "config.h"
#include "batt.h"
#include "led.h"
#include "sleep.h"
#include "session.h"
#include "pulse.h"
#include "button.h"

// Values for main state machine
enum { STATE_IDLE,
       STATE_SEND,
       STATE_SENDING,
       STATE_JOINFAILED,
       STATE_SHUTDOWN,
       STATE_SLEEP };

// LMIC pin mapping
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 6,
  .dio = {2, 3, 4},
};

// Union for payload data, we may only send the first four bytes (battery voltage and counter value) if no BMP/BME280 is connected
union payload_data_t {
  struct {
    uint16_t batt;
    uint16_t count;
    int32_t temperature;
    int32_t pressure;
#ifdef SENSOR_BME280
    int32_t humidity;
#endif
  };
  unsigned char bytes[4];
#ifndef SENSOR_BME280
  unsigned char bytes_bme[12];
#else
  unsigned char bytes_bme[14];
#endif
};

// Structure for BMP/BME280 sensor
tiny::BME280 bme;
// Set true if sensor is detected
bool bme_status;

// Uplink / downlink functions
bool raingauge_send(void);
void raingauge_receive(void);

// Global variables
static uint8_t state = STATE_SHUTDOWN;
static uint32_t delay_counter = 0;

// LMIC event handler
void onEvent(ev_t ev) {
  switch(ev) {
  case EV_JOINED:
    Serial.println(F("Join succeeded"));
    // Disable link check validation
    LMIC_setLinkCheckMode(0);
    // Store session data & frame counter immediately
    session_changed(true);
    break;
  case EV_TXCOMPLETE:
    Serial.println(F("Packet sent"));
    // Store session data & frame counter from time to time
    session_changed(false);
    // Deactivate LED
    led_disable();
    // Check downlink data
    if(LMIC.dataLen) {
      raingauge_receive();
    }
    state = STATE_SHUTDOWN;
    break;
  case EV_JOIN_TXCOMPLETE:
    Serial.println(F("Join failed"));
    // Activate flashing LED to indicate error status
    led_enable(LED_BLINK);
    state = STATE_JOINFAILED;
    break;
  default:
    break;
  }
}

// Generate uplink payload and send LoRa packet
bool raingauge_send(void) {
  payload_data_t payload;

  // Possible to send data?
  if(!(LMIC.opmode & OP_TXRXPEND)) {
    // Prepare payload
    payload.batt = battery_getvoltage();
    payload.count = pulse_get();
    if(bme_status) {
      // Wake up and read sensor
      bme.setMode(tiny::Mode::FORCED);
      delay(15);
      payload.temperature = bme.readFixedTempC();
      payload.pressure = bme.readFixedPressure();
#ifdef SENSOR_BME280
      payload.humidity = bme.readFixedHumidity();
#endif
    }
    // Payload size depends on detection of BMP/BME280 sensor
    if(!bme_status) {
      LMIC_setTxData2(1, payload.bytes, sizeof(payload.bytes), 0);
    } else {
      LMIC_setTxData2(1, payload.bytes_bme, sizeof(payload.bytes_bme), 0);
    }
    Serial.println(F("Packet queued"));
    // Activate LED to indicate TX in progress
    led_enable(LED_ON);
  } else {
    delay(1);
    return false;
  }
  return true;
}

// Handle downlink payload
void raingauge_receive(void) {
  switch(LMIC.frame[LMIC.dataBeg - 1]) {
  case 1: // fPort 1 - Update sleep interval
    if(LMIC.dataLen == 2) {
      uint32_t value = (LMIC.frame[LMIC.dataBeg] << 8) | LMIC.frame[LMIC.dataBeg + 1];
      sleep_set(value);
      session_changed(true);
    }
    break;
  case 2: // fPort 2 - Update rain counter
    if(LMIC.dataLen == 2) {
      uint16_t value = (LMIC.frame[LMIC.dataBeg] << 8) | LMIC.frame[LMIC.dataBeg + 1];
      pulse_set(value);
      session_changed(true);
    }
    break;
  }
}

// Arduino setup function
void setup() {
  Serial.begin(115200);
  Serial.println(F("System starting"));

  // LMIC init
  os_init();

  // Reset the MAC state. Session and pending data transfers will be discarded.
  LMIC_reset();

  // Initialize battery voltage measurement
  battery_setup();

  // Check if battery voltage is okay
  if(battery_getvoltage() < BATT_MIN) {
    // Battery voltage too low, maybe we restarted by a BOD reset;
    // sleep until user inserts fresh batteries to avoid reset loops
    Serial.println(F("Low battery"));
    Serial.flush();
    sleep_forever();
  }

  // Initialize LED
  led_setup();

  // Initialize button
  button_setup();

  // Setup rain gauge pulse counter routines
  pulse_setup();

  // Initialize sleep interval
  sleep_set(TX_INTERVAL);

  // Initialize network specific parameters
  session_setup();

  // Detect BMP/BME280 sensor at I2C address 0x77 or 0x76
  bme_status = bme.begin();
  if(!bme_status) {
    bme_status = bme.beginI2C(0x76);
  }
  if(bme_status) {
    Serial.println(F("BMx280 found"));
    bme.setMode(tiny::Mode::SLEEP);
  } else {
    Serial.println(F("BMx280 not found"));
  }
}

// Arduino main loop
void loop() {
  os_runloop_once();
  led_loop();
  // Main state machine
  switch(state) {
  case STATE_IDLE:
    // Check the battery voltage before sending data
    if(battery_getvoltage() >= BATT_MIN) {
      // Okay, send uplink data soon
      ++state;
    } else {
      // Battery empty, go to sleep
      state = STATE_SLEEP;
    }
    break;
  case STATE_SEND:
    // Prepare uplink packet
    if(raingauge_send()) {
      ++state;
    }
    break;
  case STATE_SENDING:
    // We just do nothing here, state will be changed by LMIC event handler
    break;
  case STATE_JOINFAILED:
    // If join fails, reset LMIC and go to deep sleep with some delay while the LED blinks
    session_defaults();
    delay_counter = 0xffff;
    ++state;
    break;
  case STATE_SHUTDOWN:
    // Wait until all LMIC operations are done and store session data if needed
    if(LMIC_queryTxReady()) {
      session_check_changes();
      ++state;
    }
    break;
  case STATE_SLEEP:
    // Put everything into low power mode and send MCU to deep sleep
    if(delay_counter > 0) {
      --delay_counter;
    } else {
      led_disable();
      session_sleep();
      Serial.println(F("Go to sleep"));
      Serial.flush();
      sleep_interval();
      // We'll wake up here and switch back to idle state
      session_wakeup();
      state = STATE_IDLE;
    }
    break;
  }
}