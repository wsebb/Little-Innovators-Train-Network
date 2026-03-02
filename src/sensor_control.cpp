/**
 * Sensor Control Module
 */

#include "sensor_control.h"
#include "config.h"
#include "signal_control.h"

/**
 * Determines if a track section is occupied using ultrasonic sensor
 */
bool isOccupied(int trigPin, int echoPin) {
  int validDetections = 0;
  
  for (int i = 0; i < SENSOR_SAMPLES; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH, 20000);
    
    if (duration > 0) {
      long distanceCm = duration * 0.034 / 2;
      
      if (distanceCm > 0 && distanceCm < DETECTION_THRESHOLD) {
        validDetections++;
      }
    }
    
    delay(15);
  }
  
  return (validDetections >= SENSOR_CONFIDENCE);
}

/**
 * Main sensor processing function
 */
void processSensors() {
  bool occ1 = isOccupied(TRIG1, ECHO1);
  bool occ2 = isOccupied(TRIG2, ECHO2);
  bool occ3 = isOccupied(TRIG3, ECHO3);
  bool occ4 = isOccupied(TRIG4, ECHO4);

  Serial.print(F("  Track Status: T1="));
  Serial.print(occ1 ? F("OCC") : F("CLR"));
  Serial.print(F(" T2="));
  Serial.print(occ2 ? F("OCC") : F("CLR"));
  Serial.print(F(" T3="));
  Serial.print(occ3 ? F("OCC") : F("CLR"));
  Serial.print(F(" T4="));
  Serial.println(occ4 ? F("OCC") : F("CLR"));

  processTrack1(occ1, occ2, occ4);
  processTrack2(occ2, occ4);
  processTrack3(occ3);
  processTrack4(occ4, occ2);
  processStationCapacity(occ1, occ2);
}

void processTrack1(bool occ1, bool occ2, bool occ4) {
  if (occ1 != prevOcc1) {
    if (occ1) {
      Serial.println(F("[TRACK 1] OCCUPIED"));
      setSignal(sig1Green, false, S1_G, S1_R);
    } else {
      Serial.println(F("[TRACK 1] CLEAR"));
      isLocked = true;
      lockoutTimer = millis();
      
      if (!occ2 && switchPosition == SWITCH_RIGHT && !occ4) {
        setSwitch(SWITCH_LEFT);
      }
    }
    prevOcc1 = occ1;
  }
}

void processTrack2(bool occ2, bool occ4) {
  if (occ2 != prevOcc2) {
    if (occ2) {
      Serial.println(F("[TRACK 2] OCCUPIED"));
      setSignal(sig2Green, false, S2_G, S2_R);
      setSwitch(SWITCH_RIGHT);
    } else {
      Serial.println(F("[TRACK 2] CLEAR"));
      isLocked = true;
      lockoutTimer = millis();
      
      if (!occ4) {
        setSwitch(SWITCH_LEFT);
      }
    }
    prevOcc2 = occ2;
  }
}

void processTrack3(bool occ3) {
  if (occ3 != prevOcc3) {
    if (occ3) {
      Serial.println(F("[TRACK 3] OCCUPIED - PROTECTION ACTIVE"));
      setSignal(sig1Green, false, S1_G, S1_R);
      setSignal(sig2Green, false, S2_G, S2_R);
    } else {
      Serial.println(F("[TRACK 3] CLEAR - Protection released"));
    }
    prevOcc3 = occ3;
  }
}

void processTrack4(bool occ4, bool occ2) {
  if (occ4 != prevOcc4) {
    if (occ4) {
      Serial.println(F("[TRACK 4] OCCUPIED"));
      setSignal(sig3Green, false, S3_G, S3_R);
    } else {
      Serial.println(F("[TRACK 4] CLEAR"));
      setSignal(sig3Green, true, S3_G, S3_R);
      
      if (!occ2 && switchPosition == SWITCH_RIGHT) {
        setSwitch(SWITCH_LEFT);
      }
    }
    prevOcc4 = occ4;
  }
}

void processStationCapacity(bool occ1, bool occ2) {
  bool stationFull = (occ1 && occ2);
  static bool prevStationFull = false;
  
  if (stationFull != prevStationFull) {
    if (stationFull) {
      Serial.println(F("[STATION] FULL - Signal 4 -> RED"));
      setSignal(sig4Green, false, S4_G, S4_R);
    } else {
      Serial.println(F("[STATION] AVAILABLE - Signal 4 -> GREEN"));
      setSignal(sig4Green, true, S4_G, S4_R);
    }
    prevStationFull = stationFull;
  }
}