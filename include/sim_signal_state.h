/**
 * Simulation-only signal state LED mapping
 */

#ifndef SIM_SIGNAL_STATE_H
#define SIM_SIGNAL_STATE_H

#include <Arduino.h>

const int S1_LED_G = 32; const int S1_LED_R = 33;
const int S2_LED_G = 34; const int S2_LED_R = 35;
const int S3_LED_G = 36; const int S3_LED_R = 37;
const int S4_LED_G = 38; const int S4_LED_R = 39;
const int SW_LED_L = 40; const int SW_LED_R = 41;

void initializeSignalStateLeds();
void updateSignalStateLedByRelayPin(int pinG, bool isGreen);
void initializeSwitchStateLeds();
void updateSwitchStateLeds(SwitchPosition position);

#endif // SIM_SIGNAL_STATE_H