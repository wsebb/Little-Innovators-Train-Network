/**
 * Configuration File
 * Contains all pin definitions, constants, and configuration parameters
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// PIN DEFINITIONS
// ==========================================

// Button Inputs
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
// IMPORTANT: Verify relay logic before extended operation (to prevent damage to components)
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

// ==========================================
// TIMING & DETECTION PARAMETERS
// ==========================================

// Sensor Configuration
const int DETECTION_THRESHOLD = 5;           // Distance in cm for train detection
const int SENSOR_SAMPLES = 5;                // Number of readings per sensor check
const int SENSOR_CONFIDENCE = 3;             // Minimum valid readings to confirm detection

// Timing Parameters
const unsigned long RELAY_PULSE_MS = 200;    // Relay pulse duration (milliseconds)
const unsigned long BLANKING_MS = 500;       // EMI blanking after relay activation (milliseconds)
const unsigned long LOCKOUT_DURATION = 5000; // Button lockout duration (milliseconds)
const unsigned long SENSOR_INTERVAL = 1000;  // Sensor check interval (milliseconds)
const unsigned long BUTTON_DEBOUNCE_MS = 50; // Button debounce interval (milliseconds)

enum SwitchPosition : uint8_t {
	SWITCH_LEFT = 0,
	SWITCH_RIGHT = 1
};

// ==========================================
// GLOBAL STATE VARIABLES (EXTERN)
// ==========================================

// Button States
extern bool lastBtn1State;
extern bool lastBtn2State;

// Track Occupancy States
extern bool prevOcc1;
extern bool prevOcc2;
extern bool prevOcc3;
extern bool prevOcc4;

// Signal States
extern bool sig1Green;
extern bool sig2Green;
extern bool sig3Green;
extern bool sig4Green;

// Switch Position
extern SwitchPosition switchPosition;

// Timing Variables
extern unsigned long previousSensorCheck;
extern unsigned long lockoutTimer;
extern unsigned long lastRelayMove;
extern bool isLocked;

// Relay Management
extern unsigned long relayOffTime[40];

#endif // CONFIG_H