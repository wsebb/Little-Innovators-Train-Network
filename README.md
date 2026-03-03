# Railway Signaling System

Arduino Mega project for a small train-network demo.

Built for Little Innovators (Siemens). The controller reads 4 ultrasonic sensors, drives relay outputs for signals/switches, and applies basic interlocking so unsafe routes are blocked.

## Overview

The firmware controls four logical track zones:

- **Track 1 / Track 2**: station tracks with manual green requests (buttons)
- **Track 3**: protection zone (hard interlock)
- **Track 4**: main-line movement and switch coordination

Main loop is non-blocking and cycles through:

- Relay pulse timing
- Button edge detection + debounce
- Sensor scan on interval
- Lockout/interlock checks

## Build / Run

### Real board (Mega 2560)

```bash
pio run -e megaatmega2560
pio run -e megaatmega2560 --target upload
pio device monitor
```

### Wokwi sim

```bash
pio run -e megaatmega2560_sim
```

Then run **Wokwi: Start Simulation** in VS Code.

## Notes from testing

- Relays are active-low (`RELAY_ON = LOW`, `RELAY_OFF = HIGH`)
- Relay commands are pulses, not steady outputs
- Sensor readings can spike right after relay moves, so sensor checks are blanked for a short period
- Blanking time is adaptive (`MIN_BLANKING_MS` to `MAX_BLANKING_MS`) based on how noisy the last cycle looked

Occupancy logic is intentionally conservative:

1. enter/exit thresholds for hysteresis
2. vote over multiple samples
3. minimum dwell time before state flip

## Safety behavior

- If Track 3 is occupied, station signals are forced red
- Button requests are blocked if protection is active, signal is already green, or lockout is active
- A lockout timer prevents rapid repeated commands
- Signal 4 shows if the station tracks are full

## Hardware wiring tip

If possible, power relays from a separate 5V rail and keep grounds common. If you only have one supply, add local decoupling near relay modules and sensors (bulk electrolytic + small ceramic caps).

## Pin map

### Sensors
- Track 1: `TRIG1=22`, `ECHO1=23`
- Track 2: `TRIG2=24`, `ECHO2=25`
- Track 3: `TRIG3=26`, `ECHO3=27`
- Track 4: `TRIG4=28`, `ECHO4=29`

### Signals
- Signal 1: `S1_G=2`, `S1_R=3`
- Signal 2: `S2_G=4`, `S2_R=5`
- Signal 3: `S3_G=6`, `S3_R=7`
- Signal 4: `S4_G=8`, `S4_R=9`

### Switch + buttons
- Switch: `SW_L=10`, `SW_R=11`
- Button 1: `BTN1_PIN=30`
- Button 2: `BTN2_PIN=31`

## Tuning values

Adjust in `include/config.h`:

- detection thresholds: `DETECT_ENTER_CM`, `DETECT_EXIT_CM`
- vote settings: `SENSOR_SAMPLES`, `SENSOR_CONFIDENCE`
- dwell: `TRACK_STATE_DWELL_MS`
- relay pulse: `RELAY_PULSE_MS`
- blanking: `BLANKING_MS`, `MIN_BLANKING_MS`, `MAX_BLANKING_MS`, `BLANKING_STEP_MS`
- timing: `SENSOR_INTERVAL`, `BUTTON_DEBOUNCE_MS`, `LOCKOUT_DURATION`

## Project layout

`*` = sim-only

```text
Little Innovators Train Network/
├── include/
├── src/
│   └── sim/ *
├── diagram.json *
├── platformio.ini
├── wokwi.toml *
└── README.md
```
