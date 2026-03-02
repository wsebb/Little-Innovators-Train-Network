/**
 * Sensor Control Module Header
 */

#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H

#include <Arduino.h>

bool isOccupied(int trigPin, int echoPin);
void processSensors();
void processTrack1(bool occ1, bool occ2, bool occ4);
void processTrack2(bool occ2, bool occ4);
void processTrack3(bool occ3);
void processTrack4(bool occ4, bool occ2);
void processStationCapacity(bool occ1, bool occ2);

#endif // SENSOR_CONTROL_H