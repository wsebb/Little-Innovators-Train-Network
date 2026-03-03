/**
 * @file sensor_control.h
 * @brief Sensor vote processing and occupancy-driven route logic.
 *
 * This module converts ultrasonic measurements into filtered occupancy states,
 * then applies routing and interlocking rules for each controlled track.
 *
 * Filtering pipeline:
 * 1) Sample-level hysteresis decision,
 * 2) Multi-sample vote confidence,
 * 3) Transition dwell filtering before state publication.
 */

#ifndef SENSOR_CONTROL_H
#define SENSOR_CONTROL_H

#include <Arduino.h>

/**
 * @brief Performs a direct occupancy decision for one ultrasonic pair.
 * @param trigPin Trigger pin connected to the HC-SR04 TRIG input.
 * @param echoPin Echo pin connected to the HC-SR04 ECHO output.
 * @return `true` when the sensor vote passes confidence for occupied state;
 *         otherwise `false`.
 *
 * This call does not apply track dwell filtering or transition side effects.
 */
bool isOccupied(int trigPin, int echoPin);

/**
 * @brief Executes one complete sensor scan and route update cycle.
 * @return None.
 *
 * Performs per-track vote collection, dwell filtering, instability accounting,
 * and transition logic for tracks 1-4 plus station-capacity signaling.
 */
void processSensors();

/**
 * @brief Returns instability observed in the most recent sensor cycle.
 * @return Count of tracks with borderline confidence results.
 *
 * Borderline means vote totals near the acceptance boundary and is used by
 * adaptive blanking in the main loop.
 *
 * Engineering use: a high count indicates low confidence margin after recent
 * relay movement, so blanking can be increased to protect decision quality.
 */
uint8_t getLastSensorUnstableCount();

/**
 * @brief Applies transition logic for Track 1.
 * @param occ1 Current filtered occupancy state for Track 1.
 * @param track2Occupied Current filtered occupancy state for Track 2.
 * @param occ4 Current filtered occupancy state for Track 4.
 * @return None.
 */
void processTrack1(bool occ1, bool track2Occupied, bool occ4);

/**
 * @brief Applies transition logic for Track 2.
 * @param occ2 Current filtered occupancy state for Track 2.
 * @param occ4 Current filtered occupancy state for Track 4.
 * @return None.
 */
void processTrack2(bool occ2, bool occ4);

/**
 * @brief Applies interlock logic for Track 3 (protection zone).
 * @param occ3 Current filtered occupancy state for Track 3.
 * @return None.
 *
 * When occupied, station-facing movement is forced safe by setting
 * station signals to red.
 */
void processTrack3(bool occ3);

/**
 * @brief Applies transition logic for Track 4.
 * @param occ4 Current filtered occupancy state for Track 4.
 * @param occ2 Current filtered occupancy state for Track 2.
 * @return None.
 */
void processTrack4(bool occ4, bool occ2);

/**
 * @brief Updates station-capacity indication from platform occupancy.
 * @param occ1 Current filtered occupancy state for Track 1.
 * @param occ2 Current filtered occupancy state for Track 2.
 * @return None.
 */
void processStationCapacity(bool occ1, bool occ2);

#endif // SENSOR_CONTROL_H