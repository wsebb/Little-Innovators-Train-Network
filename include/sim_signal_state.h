/**
 * @file sim_signal_state.h
 * @brief Simulation-only LED state mirrors for signals and switch position.
 *
 * These helpers provide persistent visual state in Wokwi beyond the relay pulse
 * window, making it easier to observe effective commanded state.
 */

#ifndef SIM_SIGNAL_STATE_H
#define SIM_SIGNAL_STATE_H

#include <Arduino.h>
#include "config.h"

const int S1_LED_G = 32; const int S1_LED_R = 33;
const int S2_LED_G = 34; const int S2_LED_R = 35;
const int S3_LED_G = 36; const int S3_LED_R = 37;
const int S4_LED_G = 38; const int S4_LED_R = 39;
const int SW_LED_L = 40; const int SW_LED_R = 41;

/**
 * @brief Initializes simulation LEDs for signal-state indication.
 * @return None.
 */
void initializeSignalStateLeds();

/**
 * @brief Updates signal-state LEDs by logical signal identity.
 * @param pinG Green relay pin that identifies which signal to update.
 * @param isGreen Target logical state (`true` = GREEN, `false` = RED).
 * @return None.
 */
void updateSignalStateLedByRelayPin(int pinG, bool isGreen);

/**
 * @brief Initializes simulation LEDs for switch-state indication.
 * @return None.
 */
void initializeSwitchStateLeds();

/**
 * @brief Updates switch-state LEDs for the selected turnout position.
 * @param position Requested switch position.
 * @return None.
 */
void updateSwitchStateLeds(SwitchPosition position);

#endif // SIM_SIGNAL_STATE_H