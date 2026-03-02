/**
 * Relay Control Module
 * Handles relay activation, timing, and automatic deactivation
 */

#include "relay_control.h"
#include "config.h"

/**
 * Manages automatic relay deactivation after pulse duration
 */
void manageRelayTimers(unsigned long currentMillis) {
  for (int i = 0; i < 40; i++) {
    if (relayOffTime[i] > 0 && currentMillis >= relayOffTime[i]) {
      digitalWrite(i, RELAY_OFF);
      relayOffTime[i] = 0;
    }
  }
}

/**
 * Activates a relay for a specified duration then automatically deactivates
 */
void pulsePin(int pin) {
  digitalWrite(pin, RELAY_ON);
  relayOffTime[pin] = millis() + RELAY_PULSE_MS;
  lastRelayMove = millis(); // Activate EMI blanking
  
  Serial.print(F("  [RELAY] Pin "));
  Serial.print(pin);
  Serial.println(F(" activated"));
}