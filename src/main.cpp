const char* const SYSTEM_VERSION = "3.1";

#include <Arduino.h>
#include "config.h"
#include "relay_control.h"
#include "signal_control.h"
#include "sensor_control.h"
#include "button_control.h"

// ==========================================
// GLOBAL STATE VARIABLES DEFINITION
// ==========================================

// Button States
bool lastBtn1State = HIGH;
bool lastBtn2State = HIGH;

// Track Occupancy States
bool prevOcc1 = false;
bool prevOcc2 = false;
bool prevOcc3 = false;
bool prevOcc4 = false;

// Signal States (false = RED, true = GREEN)
bool sig1Green = false;  // Station signal 1 - initial state RED
bool sig2Green = false;  // Station signal 2 - initial state RED
bool sig3Green = true;   // Open track signal 3 - initial state GREEN
bool sig4Green = true;   // Open track signal 4 - initial state GREEN

// Switch Position
SwitchPosition switchPosition = SWITCH_LEFT;

// Timing Variables
unsigned long previousSensorCheck = 0;
unsigned long lockoutTimer = 0;
unsigned long lastRelayMove = 0;
unsigned long adaptiveBlankingMs = BLANKING_MS;
bool isLocked = false;

// Relay Management Array
unsigned long relayOffTime[40] = {0};

// ==========================================
// FORWARD DECLARATIONS
// ==========================================

void initializeRelays();
void initializeButtons();
void initializeSensors();
void printStartupBanner();
void printSystemReady();
void runStartupSequence();

// ==========================================
// SETUP - INITIALIZE SYSTEM
// ==========================================

/**
 * Initializes hardware, applies startup outputs, and starts monitoring.
 */
void setup() {
  Serial.begin(9600);
  delay(100); // Allow serial communication to stabilize
  
  // Initialize hardware
  initializeRelays();
  initializeButtons();
  initializeSensors();
  
  // Print startup banner
  printStartupBanner();
  
  // Run startup sequence
  runStartupSequence();
  
  // Reset the blanking timer so first sensor scan isn't blocked
  lastRelayMove = millis() - BLANKING_MS - 1000;
  adaptiveBlankingMs = BLANKING_MS;
  
  printSystemReady();
}

// ==========================================
// MAIN LOOP
// ==========================================

/**
 * Executes non-blocking runtime tasks and scheduled sensor scans.
 */
void loop() {
  unsigned long currentMillis = millis();

  // 1. RELAY TIMER MANAGEMENT (Non-blocking)
  manageRelayTimers(currentMillis);

  // 2. LOCKOUT TIMER MANAGEMENT
  if (isLocked && (currentMillis - lockoutTimer >= LOCKOUT_DURATION)) {
    if (sig1Green) {
      Serial.println(F("[SYSTEM] Signal 1 timeout -> RED"));
      setSignal(sig1Green, false, S1_G, S1_R);
    }

    if (sig2Green) {
      Serial.println(F("[SYSTEM] Signal 2 timeout -> RED"));
      setSignal(sig2Green, false, S2_G, S2_R);
    }

    isLocked = false;
    Serial.println(F("[SYSTEM] Lockout released"));
  }

  // 3. BUTTON MONITORING (Always active)
  checkButtons(currentMillis);

  // 4. SENSOR MONITORING (With EMI blanking)
  if (currentMillis - previousSensorCheck >= SENSOR_INTERVAL) {
    previousSensorCheck = currentMillis;
    unsigned long sinceRelay = currentMillis - lastRelayMove;

    // Check if we're in the EMI blanking period
    if (sinceRelay < adaptiveBlankingMs) {
      Serial.print(F("[EMI] Blanking Active ("));
      Serial.print(adaptiveBlankingMs - sinceRelay);
      Serial.println(F("ms remaining) - Skipping sensor scan"));
      return; // Skip this sensor cycle
    }

    Serial.println(F("[SENSOR] Scanning sensors..."));
    processSensors();

    if (sinceRelay <= adaptiveBlankingMs + SENSOR_INTERVAL) {
      uint8_t unstableTracks = getLastSensorUnstableCount();
      unsigned long previousBlanking = adaptiveBlankingMs;

      if (unstableTracks >= 2 && adaptiveBlankingMs < MAX_BLANKING_MS) {
        adaptiveBlankingMs += BLANKING_STEP_MS;
        if (adaptiveBlankingMs > MAX_BLANKING_MS) {
          adaptiveBlankingMs = MAX_BLANKING_MS;
        }
      } else if (unstableTracks == 0 && adaptiveBlankingMs > MIN_BLANKING_MS) {
        if (adaptiveBlankingMs > BLANKING_STEP_MS) {
          adaptiveBlankingMs -= BLANKING_STEP_MS;
        }
        if (adaptiveBlankingMs < MIN_BLANKING_MS) {
          adaptiveBlankingMs = MIN_BLANKING_MS;
        }
      }

      if (adaptiveBlankingMs != previousBlanking) {
        Serial.print(F("[EMI] Adaptive blanking -> "));
        Serial.print(adaptiveBlankingMs);
        Serial.print(F("ms (unstable tracks="));
        Serial.print(unstableTracks);
        Serial.println(F(")"));
      }
    }
  }
}

// ==========================================
// INITIALIZATION FUNCTIONS
// ==========================================

/**
 * Configures relay pins and sets all relays to inactive state.
 */
void initializeRelays() {
  int relayPins[] = {S1_G, S1_R, S2_G, S2_R, S3_G, S3_R, S4_G, S4_R, SW_L, SW_R};
  for (int i = 0; i < 10; i++) {
    digitalWrite(relayPins[i], RELAY_OFF);
    pinMode(relayPins[i], OUTPUT);
    relayOffTime[relayPins[i]] = 0;
  }
}

/**
 * Configures button input pins.
 */
void initializeButtons() {
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
}

/**
 * Configures ultrasonic sensor trigger and echo pins.
 */
void initializeSensors() {
  pinMode(TRIG1, OUTPUT); pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT); pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT); pinMode(ECHO3, INPUT);
  pinMode(TRIG4, OUTPUT); pinMode(ECHO4, INPUT);

  digitalWrite(TRIG1, LOW);
  digitalWrite(TRIG2, LOW);
  digitalWrite(TRIG3, LOW);
  digitalWrite(TRIG4, LOW);
}

/**
 * Prints the startup banner to serial output.
 */
void printStartupBanner() {
  Serial.println(F(""));
  Serial.println(F("========================================="));
  Serial.print(F("  Railway Signaling System v"));
  Serial.println(SYSTEM_VERSION);
  Serial.println(F("========================================="));
  Serial.println(F(""));
  Serial.println(F("Initializing system components..."));
  Serial.println(F(""));
}

/**
 * Prints final startup status once initialization is complete.
 */
void printSystemReady() {
  Serial.println(F(""));
  Serial.println(F("Initialization complete"));
  Serial.println(F("All signals and switch configured"));
  Serial.println(F("System operational - Starting sensor monitoring"));
  Serial.println(F(""));
}

/**
 * Drives initial signal and switch positions at boot.
 */
void runStartupSequence() {
  // Signal 1 -> RED
  Serial.println(F("-> [1/5] Setting Signal 1 to RED"));
  digitalWrite(S1_R, RELAY_ON);
  delay(RELAY_PULSE_MS);
  digitalWrite(S1_R, RELAY_OFF);
  delay(300);

  // Signal 2 -> RED
  Serial.println(F("-> [2/5] Setting Signal 2 to RED"));
  digitalWrite(S2_R, RELAY_ON);
  delay(RELAY_PULSE_MS);
  digitalWrite(S2_R, RELAY_OFF);
  delay(300);

  // Signal 3 -> GREEN
  Serial.println(F("-> [3/5] Setting Signal 3 to GREEN"));
  digitalWrite(S3_G, RELAY_ON);
  delay(RELAY_PULSE_MS);
  digitalWrite(S3_G, RELAY_OFF);
  delay(300);

  // Signal 4 -> GREEN
  Serial.println(F("-> [4/5] Setting Signal 4 to GREEN"));
  digitalWrite(S4_G, RELAY_ON);
  delay(RELAY_PULSE_MS);
  digitalWrite(S4_G, RELAY_OFF);
  delay(300);

  // Switch -> LEFT
  Serial.println(F("-> [5/5] Setting Switch to LEFT"));
  digitalWrite(SW_L, RELAY_ON);
  delay(RELAY_PULSE_MS);
  digitalWrite(SW_L, RELAY_OFF);
  delay(500);
}