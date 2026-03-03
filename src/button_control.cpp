/** Button input handling and request gating. */

#include "button_control.h"
#include "config.h"
#include "signal_control.h"

/**
 * @brief Polls both buttons for rising-edge press events.
 * @param now Current system time in milliseconds.
 */
void checkButtons(unsigned long now) {
  checkButton1(now);
  checkButton2(now);
}

/**
 * @brief Processes button 1 press requests for signal 1.
 * @param now Current system time in milliseconds.
 */
void checkButton1(unsigned long now) {
  static unsigned long lastBtn1PressTime = 0;
  bool btn1Current = digitalRead(BTN1_PIN);
  if (btn1Current == LOW && lastBtn1State == HIGH) {
    if (now - lastBtn1PressTime < BUTTON_DEBOUNCE_MS) {
      lastBtn1State = btn1Current;
      return;
    }
    lastBtn1PressTime = now;

    if (!sig1Green && !prevOcc3 && !isLocked) {
      Serial.println(F("[BUTTON] Button 1 pressed - Requesting Signal 1 GREEN"));
      setSignal(sig1Green, true, S1_G, S1_R);
      isLocked = true;
      lockoutTimer = now;
    } else {
      Serial.print(F("[BUTTON] Button 1 pressed - BLOCKED ("));
      if (sig1Green) Serial.print(F("already green, "));
      if (prevOcc3) Serial.print(F("track 3 occupied, "));
      if (isLocked) Serial.print(F("system locked"));
      Serial.println(F(")"));
    }
  }
  lastBtn1State = btn1Current;
}

/**
 * @brief Processes button 2 press requests for signal 2.
 * @param now Current system time in milliseconds.
 */
void checkButton2(unsigned long now) {
  static unsigned long lastBtn2PressTime = 0;
  bool btn2Current = digitalRead(BTN2_PIN);
  if (btn2Current == LOW && lastBtn2State == HIGH) {
    if (now - lastBtn2PressTime < BUTTON_DEBOUNCE_MS) {
      lastBtn2State = btn2Current;
      return;
    }
    lastBtn2PressTime = now;

    if (!sig2Green && !prevOcc3 && !isLocked) {
      Serial.println(F("[BUTTON] Button 2 pressed - Requesting Signal 2 GREEN"));
      setSignal(sig2Green, true, S2_G, S2_R);
      isLocked = true;
      lockoutTimer = now;
    } else {
      Serial.print(F("[BUTTON] Button 2 pressed - BLOCKED ("));
      if (sig2Green) Serial.print(F("already green, "));
      if (prevOcc3) Serial.print(F("track 3 occupied, "));
      if (isLocked) Serial.print(F("system locked"));
      Serial.println(F(")"));
    }
  }
  lastBtn2State = btn2Current;
}