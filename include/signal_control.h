/**
 * Signal and Switch Control Module Header
 */

#ifndef SIGNAL_CONTROL_H
#define SIGNAL_CONTROL_H

#include <Arduino.h>
#include "config.h"

void setSignal(bool &currentState, bool newState, int pinG, int pinR);
void setSwitch(SwitchPosition newPos);

#endif // SIGNAL_CONTROL_H