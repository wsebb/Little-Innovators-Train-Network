/**
 * Signal and Switch Control Module (Simulation variant)
 */

#include "signal_control.h"
#include "config.h"
#include "relay_control.h"
#include "sim_signal_state.h"

void updateSignalStateLedByRelayPin(int pinG, bool isGreen) {
  int greenLedPin = -1;
  int redLedPin = -1;

  if (pinG == S1_G) {
    greenLedPin = S1_LED_G;
    redLedPin = S1_LED_R;
  } else if (pinG == S2_G) {
    greenLedPin = S2_LED_G;
    redLedPin = S2_LED_R;
  } else if (pinG == S3_G) {
    greenLedPin = S3_LED_G;
    redLedPin = S3_LED_R;
  } else if (pinG == S4_G) {
    greenLedPin = S4_LED_G;
    redLedPin = S4_LED_R;
  }

  if (greenLedPin >= 0 && redLedPin >= 0) {
    digitalWrite(greenLedPin, isGreen ? HIGH : LOW);
    digitalWrite(redLedPin, isGreen ? LOW : HIGH);
  }
}

void initializeSignalStateLeds() {
  pinMode(S1_LED_G, OUTPUT); pinMode(S1_LED_R, OUTPUT);
  pinMode(S2_LED_G, OUTPUT); pinMode(S2_LED_R, OUTPUT);
  pinMode(S3_LED_G, OUTPUT); pinMode(S3_LED_R, OUTPUT);
  pinMode(S4_LED_G, OUTPUT); pinMode(S4_LED_R, OUTPUT);

  updateSignalStateLedByRelayPin(S1_G, sig1Green);
  updateSignalStateLedByRelayPin(S2_G, sig2Green);
  updateSignalStateLedByRelayPin(S3_G, sig3Green);
  updateSignalStateLedByRelayPin(S4_G, sig4Green);
}

void updateSwitchStateLeds(SwitchPosition position) {
  digitalWrite(SW_LED_L, position == SWITCH_LEFT ? HIGH : LOW);
  digitalWrite(SW_LED_R, position == SWITCH_RIGHT ? HIGH : LOW);
}

void initializeSwitchStateLeds() {
  pinMode(SW_LED_L, OUTPUT);
  pinMode(SW_LED_R, OUTPUT);
  updateSwitchStateLeds(switchPosition);
}

void setSignal(bool &currentState, bool newState, int pinG, int pinR) {
  if (currentState != newState) {
    currentState = newState;
    Serial.print(F("  [SIGNAL] "));
    Serial.print(pinG == S1_G ? "1" : pinG == S2_G ? "2" : pinG == S3_G ? "3" : "4");
    Serial.print(F(" -> "));
    Serial.println(newState ? F("GREEN") : F("RED"));
    pulsePin(newState ? pinG : pinR);
    updateSignalStateLedByRelayPin(pinG, newState);
  }
}

void setSwitch(SwitchPosition newPos) {
  if (switchPosition != newPos) {
    SwitchPosition oldPos = switchPosition;
    switchPosition = newPos;
    Serial.print(F("  [SWITCH] "));
    Serial.print(oldPos == SWITCH_LEFT ? F("LEFT") : F("RIGHT"));
    Serial.print(F(" -> "));
    Serial.println(newPos == SWITCH_LEFT ? F("LEFT") : F("RIGHT"));
    pulsePin(newPos == SWITCH_LEFT ? SW_L : SW_R);
    updateSwitchStateLeds(newPos);
  }
}