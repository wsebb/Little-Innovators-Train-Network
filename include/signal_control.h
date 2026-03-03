/**
 * @file signal_control.h
 * @brief Signal and turnout (switch) actuation API.
 *
 * Provides state-aware wrappers that pulse only when a transition is required.
 * This prevents redundant relay commands and keeps serial event logs aligned
 * with actual state changes.
 */

#ifndef SIGNAL_CONTROL_H
#define SIGNAL_CONTROL_H

#include <Arduino.h>
#include "config.h"

/**
 * @brief Applies a signal transition and pulses the appropriate relay.
 * @param currentState Reference to current software state (`true` = GREEN,
 *        `false` = RED). Updated in-place on transition.
 * @param newState Requested target state (`true` = GREEN, `false` = RED).
 * @param pinG Relay pin that commands GREEN for this signal.
 * @param pinR Relay pin that commands RED for this signal.
 * @return None.
 */
void setSignal(bool &currentState, bool newState, int pinG, int pinR);

/**
 * @brief Applies a turnout position transition and pulses the matching relay.
 * @param newPos Target switch position (`SWITCH_LEFT` or `SWITCH_RIGHT`).
 * @return None.
 */
void setSwitch(SwitchPosition newPos);

#endif // SIGNAL_CONTROL_H