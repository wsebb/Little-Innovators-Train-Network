// Button handling for manual signal requests.

#ifndef BUTTON_CONTROL_H
#define BUTTON_CONTROL_H

#include <Arduino.h>

// Poll both station request buttons.
void checkButtons(unsigned long now);

// Handle button 1 press (request Signal 1).
void checkButton1(unsigned long now);

// Handle button 2 press (request Signal 2).
void checkButton2(unsigned long now);

#endif // BUTTON_CONTROL_H