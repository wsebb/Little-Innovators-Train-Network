/** Relay pulse timing and deactivation logic. */

#include "relay_control.h"
#include "config.h"

/**
 * @brief Deactivates relays whose pulse timeout has elapsed.
 * @param currentMillis Current system time in milliseconds.
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
 * @brief Starts a timed relay pulse and arms EMI blanking.
 * @param pin Relay pin to pulse.
 */
void pulsePin(int pin) {
  if (pin < 0 || pin >= 40) {
    Serial.print(F("  [RELAY] Invalid pin: "));
    Serial.println(pin);
    return;
  }

  digitalWrite(pin, RELAY_ON);
  relayOffTime[pin] = millis() + RELAY_PULSE_MS;
  lastRelayMove = millis(); // Activate EMI blanking
  
  Serial.print(F("  [RELAY] Pin "));
  Serial.print(pin);
  Serial.println(F(" activated"));
}