/**
 * @file relay_control.h
 * @brief Timed pulse control for active-low relay outputs.
 *
 * Relay coils are pulsed for a fixed time window rather than held continuously.
 * This module manages pulse lifetimes and cleanly deactivates outputs when the
 * timeout expires.
 */

#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include <Arduino.h>

/**
 * @brief Expires relay pulses whose scheduled off-time has elapsed.
 * @param currentMillis Current monotonic timestamp from millis().
 * @return None.
 *
 * Iterates the relay timing table and writes RELAY_OFF to any pin whose
 * pulse window is complete.
 */
void manageRelayTimers(unsigned long currentMillis);

/**
 * @brief Starts a relay pulse on a single output pin.
 * @param pin Arduino digital pin connected to a relay input.
 * @return None.
 *
 * On valid pin numbers, drives RELAY_ON immediately and schedules automatic
 * deactivation after RELAY_PULSE_MS. Also updates relay-move timing used by
 * sensor blanking logic.
 */
void pulsePin(int pin);

#endif // RELAY_CONTROL_H