/**
 * @file config.h
 * @brief Hardware mapping, timing constants, and shared runtime state.
 *
 * This header centralizes project-level configuration so control modules can
 * share one consistent pin map and behavioral tuning set.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// PIN DEFINITIONS
// ==========================================
// All pins are Arduino Mega digital pins.

// Button Inputs (active-low, INPUT_PULLUP)
const int BTN1_PIN = 30;  // Manual request for Signal 1
const int BTN2_PIN = 31;  // Manual request for Signal 2

// Ultrasonic Sensor Pins (Trigger/Echo pairs)
const int TRIG1 = 22; const int ECHO1 = 23;  // Track 1 sensor
const int TRIG2 = 24; const int ECHO2 = 25;  // Track 2 sensor
const int TRIG3 = 26; const int ECHO3 = 27;  // Track 3 sensor (protection zone)
const int TRIG4 = 28; const int ECHO4 = 29;  // Track 4 sensor

// Signal Control Relays (Green/Red pairs)
const int S1_G = 2;  const int S1_R = 3;   // Signal 1 Green/Red
const int S2_G = 4;  const int S2_R = 5;   // Signal 2 Green/Red
const int S3_G = 6;  const int S3_R = 7;   // Signal 3 Green/Red
const int S4_G = 8;  const int S4_R = 9;   // Signal 4 Green/Red

// Switch Control Relays
const int SW_L = 10;  // Switch position LEFT
const int SW_R = 11;  // Switch position RIGHT

// ==========================================
// RELAY CONTROL LOGIC
// ==========================================
// Relay modules are wired active-low.
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

// ==========================================
// TIMING & DETECTION PARAMETERS
// ==========================================

// Sensor Configuration
const int DETECT_ENTER_CM = 5;               // Occupied-entry threshold (cm)
const int DETECT_EXIT_CM = 6;                // Occupied-exit threshold (cm); larger than enter for hysteresis
const int SENSOR_SAMPLES = 5;                // Number of samples per sensor cycle
const int SENSOR_CONFIDENCE = 3;             // Minimum positive votes required for occupied decision
const unsigned long TRACK_STATE_DWELL_MS = 1500; // Minimum time before accepting opposite occupancy state

// Detection stack intent:
// 1) Hysteresis (ENTER vs EXIT) avoids rapid toggling around a single threshold.
// 2) Vote confidence rejects single-sample outliers.
// 3) Dwell time prevents relay thrash from short occupancy flicker.

// Timing Parameters
const unsigned long RELAY_PULSE_MS = 200;    // Relay pulse width
const unsigned long BLANKING_MS = 500;       // Initial blanking after relay movement
const unsigned long MIN_BLANKING_MS = 300;   // Lower limit for adaptive blanking
const unsigned long MAX_BLANKING_MS = 1200;  // Upper limit for adaptive blanking
const unsigned long BLANKING_STEP_MS = 100;  // Step size for adaptive blanking adjustments
const unsigned long LOCKOUT_DURATION = 5000; // Command lockout interval after granted movement requests
const unsigned long SENSOR_INTERVAL = 1000;  // Sensor polling period
const unsigned long BUTTON_DEBOUNCE_MS = 50; // Debounce window for button edge detection

// Adaptive blanking rationale:
// Relay transitions can inject transient noise into ultrasonic echoes.
// Blanking prevents immediate post-relay readings from driving occupancy logic.
// Runtime code adjusts blanking within [MIN, MAX] based on recent instability.
// Instability is inferred from borderline vote outcomes near SENSOR_CONFIDENCE.

enum SwitchPosition : uint8_t {
	SWITCH_LEFT = 0,
	SWITCH_RIGHT = 1
};

// ==========================================
// GLOBAL STATE VARIABLES (EXTERN)
// ==========================================

// Button States (previous sampled logic level, for edge detection)
extern bool lastBtn1State;
extern bool lastBtn2State;

// Track Occupancy States (previous filtered occupancy per track)
extern bool prevOcc1;
extern bool prevOcc2;
extern bool prevOcc3;
extern bool prevOcc4;

// Signal States (false = RED, true = GREEN)
extern bool sig1Green;
extern bool sig2Green;
extern bool sig3Green;
extern bool sig4Green;

// Switch Position (last commanded turnout state)
extern SwitchPosition switchPosition;

// Timing Variables
extern unsigned long previousSensorCheck;
extern unsigned long lockoutTimer;
extern unsigned long lastRelayMove;          // Timestamp of latest relay activation
extern bool isLocked;

// Relay Management: off-time scheduler indexed by digital pin number.
extern unsigned long relayOffTime[40];

#endif // CONFIG_H