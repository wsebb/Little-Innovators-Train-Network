/** Sensor voting, filtering, and occupancy-driven route logic. */

#include "sensor_control.h"
#include "config.h"
#include "signal_control.h"

static uint8_t lastSensorUnstableCount = 0;

/**
 * Evaluates one distance sample using hysteresis thresholds.
 */
static bool sampleOccupied(long distanceCm, bool wasOccupied) {
  if (wasOccupied) {
    return distanceCm > 0 && distanceCm <= DETECT_EXIT_CM;
  }

  return distanceCm > 0 && distanceCm < DETECT_ENTER_CM;
}

/**
 * Collects SENSOR_SAMPLES votes and returns an occupied/clear decision.
 */
static bool readOccupiedVotes(int trigPin, int echoPin, bool wasOccupied, int &validDetections) {
  validDetections = 0;

  for (int i = 0; i < SENSOR_SAMPLES; i++) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH, 20000);
    
    if (duration > 0) {
      long distanceCm = duration * 0.034 / 2;
      
      if (sampleOccupied(distanceCm, wasOccupied)) {
        validDetections++;
      }
    }
    
    delay(15);
  }
  
  return (validDetections >= SENSOR_CONFIDENCE);
}

/**
 * Performs a direct occupancy check without track-state context.
 */
bool isOccupied(int trigPin, int echoPin) {
  int validDetections = 0;
  return readOccupiedVotes(trigPin, echoPin, false, validDetections);
}

/**
 * Returns the instability count from the last sensor cycle.
 */
uint8_t getLastSensorUnstableCount() {
  return lastSensorUnstableCount;
}

/**
 * Runs one sensor cycle, applies dwell filtering, then triggers track logic.
 */
void processSensors() {
  unsigned long now = millis();
  static bool filteredOcc1 = false;
  static bool filteredOcc2 = false;
  static bool filteredOcc3 = false;
  static bool filteredOcc4 = false;
  static unsigned long lastChange1 = 0;
  static unsigned long lastChange2 = 0;
  static unsigned long lastChange3 = 0;
  static unsigned long lastChange4 = 0;

  int votes1 = 0;
  int votes2 = 0;
  int votes3 = 0;
  int votes4 = 0;

  bool rawOcc1 = readOccupiedVotes(TRIG1, ECHO1, filteredOcc1, votes1);
  bool rawOcc2 = readOccupiedVotes(TRIG2, ECHO2, filteredOcc2, votes2);
  bool rawOcc3 = readOccupiedVotes(TRIG3, ECHO3, filteredOcc3, votes3);
  bool rawOcc4 = readOccupiedVotes(TRIG4, ECHO4, filteredOcc4, votes4);

  if (rawOcc1 != filteredOcc1 && (now - lastChange1 >= TRACK_STATE_DWELL_MS)) {
    filteredOcc1 = rawOcc1;
    lastChange1 = now;
  }

  if (rawOcc2 != filteredOcc2 && (now - lastChange2 >= TRACK_STATE_DWELL_MS)) {
    filteredOcc2 = rawOcc2;
    lastChange2 = now;
  }

  if (rawOcc3 != filteredOcc3 && (now - lastChange3 >= TRACK_STATE_DWELL_MS)) {
    filteredOcc3 = rawOcc3;
    lastChange3 = now;
  }

  if (rawOcc4 != filteredOcc4 && (now - lastChange4 >= TRACK_STATE_DWELL_MS)) {
    filteredOcc4 = rawOcc4;
    lastChange4 = now;
  }

  bool occ1 = filteredOcc1;
  bool occ2 = filteredOcc2;
  bool occ3 = filteredOcc3;
  bool occ4 = filteredOcc4;

  lastSensorUnstableCount = 0;
  if (votes1 == SENSOR_CONFIDENCE || votes1 == SENSOR_CONFIDENCE - 1) lastSensorUnstableCount++;
  if (votes2 == SENSOR_CONFIDENCE || votes2 == SENSOR_CONFIDENCE - 1) lastSensorUnstableCount++;
  if (votes3 == SENSOR_CONFIDENCE || votes3 == SENSOR_CONFIDENCE - 1) lastSensorUnstableCount++;
  if (votes4 == SENSOR_CONFIDENCE || votes4 == SENSOR_CONFIDENCE - 1) lastSensorUnstableCount++;

  Serial.print(F("  Track Status: T1="));
  Serial.print(occ1 ? F("OCC") : F("CLR"));
  Serial.print(F(" T2="));
  Serial.print(occ2 ? F("OCC") : F("CLR"));
  Serial.print(F(" T3="));
  Serial.print(occ3 ? F("OCC") : F("CLR"));
  Serial.print(F(" T4="));
  Serial.println(occ4 ? F("OCC") : F("CLR"));

  processTrack3(occ3);
  processTrack1(occ1, occ2, occ4);
  processTrack2(occ2, occ4);
  processTrack4(occ4, occ2);
  processStationCapacity(occ1, occ2);
}

/**
 * Handles state transitions and actions for track 1.
 */
void processTrack1(bool occ1, bool track2Occupied, bool occ4) {
  if (occ1 != prevOcc1) {
    if (occ1) {
      Serial.println(F("[TRACK 1] OCCUPIED"));
      setSignal(sig1Green, false, S1_G, S1_R);
    } else {
      Serial.println(F("[TRACK 1] CLEAR"));
      isLocked = true;
      lockoutTimer = millis();
      
      if (!track2Occupied && switchPosition == SWITCH_RIGHT && !occ4) {
        setSwitch(SWITCH_LEFT);
      }
    }
    prevOcc1 = occ1;
  }
}

/**
 * Handles state transitions and actions for track 2.
 */
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

/**
 * Handles protection-zone transitions for track 3.
 */
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

/**
 * Handles state transitions and actions for track 4.
 */
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

/**
 * Updates station capacity signal state.
 */
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