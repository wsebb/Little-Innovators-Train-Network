/**
 * Relay Control Module Header
 */

#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <Arduino.h>

void manageRelayTimers(unsigned long currentMillis);
void pulsePin(int pin);

#endif // RELAY_CONTROL_H