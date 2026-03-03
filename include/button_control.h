/**
 * @file button_control.h
 * @brief Button polling and manual route-request gating.
 *
 * Buttons are wired as active-low inputs with pull-ups enabled.
 * The implementation detects edge-triggered presses, applies debounce,
 * and validates requests against interlock and lockout conditions.
 */

#ifndef BUTTON_CONTROL_H
#define BUTTON_CONTROL_H

#include <Arduino.h>

/**
 * @brief Polls both station request buttons.
 * @param now Current monotonic timestamp from millis().
 * @return None.
 *
 * Calls both button-specific handlers. Intended to be called every loop cycle.
 */
void checkButtons(unsigned long now);

/**
 * @brief Handles a press event for Button 1 (Signal 1 request).
 * @param now Current monotonic timestamp from millis().
 * @return None.
 *
 * Request is accepted only when Signal 1 is currently red, Track 3
 * protection is clear, and system lockout is inactive.
 */
void checkButton1(unsigned long now);

/**
 * @brief Handles a press event for Button 2 (Signal 2 request).
 * @param now Current monotonic timestamp from millis().
 * @return None.
 *
 * Request is accepted only when Signal 2 is currently red, Track 3
 * protection is clear, and system lockout is inactive.
 */
void checkButton2(unsigned long now);

#endif // BUTTON_CONTROL_H