/** Signal and switch relay actuation logic. */

#include "signal_control.h"
#include "config.h"
#include "relay_control.h"

/**
 * Applies a signal state transition and pulses the required relay.
 */
void setSignal(bool &currentState, bool newState, int pinG, int pinR) {
  if (currentState != newState) {
    currentState = newState;
    Serial.print(F("  [SIGNAL] "));
    Serial.print(pinG == S1_G ? "1" : pinG == S2_G ? "2" : pinG == S3_G ? "3" : "4");
    Serial.print(F(" -> "));
    Serial.println(newState ? F("GREEN") : F("RED"));
    pulsePin(newState ? pinG : pinR);
  }
}

/**
 * Applies a switch position transition and pulses the required relay.
 */
void setSwitch(SwitchPosition newPos) {
  if (switchPosition != newPos) {
    SwitchPosition oldPos = switchPosition;
    switchPosition = newPos;
    Serial.print(F("  [SWITCH] "));
    Serial.print(oldPos == SWITCH_LEFT ? F("LEFT") : F("RIGHT"));
    Serial.print(F(" -> "));
    Serial.println(newPos == SWITCH_LEFT ? F("LEFT") : F("RIGHT"));
    pulsePin(newPos == SWITCH_LEFT ? SW_L : SW_R);
  }
}