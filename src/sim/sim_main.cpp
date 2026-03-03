/** Simulation firmware entry point and runtime loop. */

#include <Arduino.h>
#include "config.h"
#include "relay_control.h"
#include "signal_control.h"
#include "sensor_control.h"
#include "button_control.h"
#include "sim_signal_state.h"

bool lastBtn1State = HIGH;
bool lastBtn2State = HIGH;

bool prevOcc1 = false;
bool prevOcc2 = false;
bool prevOcc3 = false;
bool prevOcc4 = false;

bool sig1Green = false;
bool sig2Green = false;
bool sig3Green = true;
bool sig4Green = true;

SwitchPosition switchPosition = SWITCH_LEFT;

unsigned long previousSensorCheck = 0;
unsigned long lockoutTimer = 0;
unsigned long lastRelayMove = 0;
unsigned long adaptiveBlankingMs = BLANKING_MS;
bool isLocked = false;

unsigned long relayOffTime[40] = {0};

void initializeRelays();
void initializeButtons();
void initializeSensors();
void printStartupBanner();
void printSystemReady();
void runStartupSequence();

/**
 * Initializes simulated hardware state and starts monitoring.
 */
void setup() {
  Serial.begin(9600);
  delay(100);

  initializeRelays();
  initializeSignalStateLeds();
  initializeSwitchStateLeds();
  initializeButtons();
  initializeSensors();

  printStartupBanner();
  runStartupSequence();

  lastRelayMove = millis() - BLANKING_MS - 1000;
  adaptiveBlankingMs = BLANKING_MS;

  printSystemReady();
}

/**
 * Executes simulation runtime tasks and scheduled sensor scans.
 */
void loop() {
  unsigned long currentMillis = millis();

  manageRelayTimers(currentMillis);

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

  checkButtons(currentMillis);

  if (currentMillis - previousSensorCheck >= SENSOR_INTERVAL) {
    previousSensorCheck = currentMillis;
    unsigned long sinceRelay = currentMillis - lastRelayMove;

    if (sinceRelay < adaptiveBlankingMs) {
      Serial.print(F("[EMI] Blanking Active ("));
      Serial.print(adaptiveBlankingMs - sinceRelay);
      Serial.println(F("ms remaining) - Skipping sensor scan"));
      return;
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
 * Prints the simulation startup banner.
 */
void printStartupBanner() {
  Serial.println(F(""));
  Serial.println(F("========================================="));
  Serial.println(F("  Railway Signaling System v2.1        "));
  Serial.println(F("  SIMULATION BUILD (LATCHED LEDs)      "));
  Serial.println(F("  EMI Blanking: 500ms                  "));
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
  Serial.println(F("-> [1/5] Setting Signal 1 to RED"));
  digitalWrite(S1_R, RELAY_ON);
  delay(RELAY_PULSE_MS);
  digitalWrite(S1_R, RELAY_OFF);
  delay(300);

  Serial.println(F("-> [2/5] Setting Signal 2 to RED"));
  digitalWrite(S2_R, RELAY_ON);
  delay(RELAY_PULSE_MS);
  digitalWrite(S2_R, RELAY_OFF);
  delay(300);

  Serial.println(F("-> [3/5] Setting Signal 3 to GREEN"));
  digitalWrite(S3_G, RELAY_ON);
  delay(RELAY_PULSE_MS);
  digitalWrite(S3_G, RELAY_OFF);
  delay(300);

  Serial.println(F("-> [4/5] Setting Signal 4 to GREEN"));
  digitalWrite(S4_G, RELAY_ON);
  delay(RELAY_PULSE_MS);
  digitalWrite(S4_G, RELAY_OFF);
  delay(300);

  Serial.println(F("-> [5/5] Setting Switch to LEFT"));
  digitalWrite(SW_L, RELAY_ON);
  delay(RELAY_PULSE_MS);
  digitalWrite(SW_L, RELAY_OFF);
  delay(500);
}