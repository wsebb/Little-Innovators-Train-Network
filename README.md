# Railway Signaling System

Automated railway signaling control for an Arduino Mega, with ultrasonic train detection, relay-driven signal/switch actuation, and safety interlocking.

Made for Little Innovators (Siemens).

## Prerequisites

- PlatformIO (CLI or VSCode extension).
- If you want simulation: Wokwi VS Code extension and a Wokwi account.

## Overview

The firmware controls four logical track zones:

- **Track 1 / Track 2**: station tracks with manual green requests (buttons),
- **Track 3**: protection zone (hard interlock),
- **Track 4**: main-line movement and switch coordination.

Core behavior is implemented as a non-blocking loop with:

- Timed relay pulses,
- Debounced button requests,
- Periodic sensor scans,
- Lockout and interlock rules,
- EMI-aware sensor blanking.

## Build and Run

### Hardware target (Arduino Mega 2560)

```bash
pio run -e megaatmega2560
pio run -e megaatmega2560 --target upload
pio device monitor
```

### Wokwi simulation target

```bash
pio run -e megaatmega2560_sim
```

Then run **Wokwi: Start Simulation** from the VS Code command palette.

## Sensor Noise Avoidance

### Relay logic and pulse operation

- Relay outputs are **active-low** (`RELAY_ON = LOW`, `RELAY_OFF = HIGH`),
- Each command is a pulse (`RELAY_PULSE_MS`) rather than a latched software state on the output pin,
- `relayOffTime[]` and `manageRelayTimers()` enforce pulse end-times without blocking the main loop.

### Sensor robustness strategy

Occupancy detection uses three layers to reduce false transitions:

1. **Differing thresholds** (`DETECT_ENTER_CM`, `DETECT_EXIT_CM`) avoid chatter near the boundary,
2. **Vote confidence** (`SENSOR_SAMPLES`, `SENSOR_CONFIDENCE`) requires multiple positive samples,
3. **State dwell filter** (`TRACK_STATE_DWELL_MS`) enforces minimum hold time before changing track state.

### EMI blanking and adaptive blanking

Relay switching can inject electrical noise into ultrasonic echo measurements. To avoid evaluating disturbed sensor frames:

- After each relay movement, sensor scanning is paused for `adaptiveBlankingMs`.
- `adaptiveBlankingMs` starts at `BLANKING_MS`.
- If recent scans show unstable vote patterns (borderline confidence on at least two tracks), blanking increases in `BLANKING_STEP_MS` increments up to `MAX_BLANKING_MS`.
- If scans are stable (zero unstable tracks), blanking decreases in `BLANKING_STEP_MS` increments down to `MIN_BLANKING_MS`.

This gives a practical self-tuning balance between responsiveness and noise immunity.

## Safety and Interlocking

- **Track 3 interlock**: when occupied, both station signals are forced red.
- **Manual request gating**: button requests are rejected when protection is active, the requested signal is already green, or lockout is active.
- **Lockout timer**: after controlled transitions, lockout prevents rapid re-commands (`LOCKOUT_DURATION`).
- **Station capacity indication**: signal 4 reflects platform availability from Tracks 1 and 2 occupancy.

## Notes from testing

Relay switching is a common source of sensor instability. Preferred wiring:

- Dedicated 5V rail for relay modules,
- Separate logic/sensor supply (USB/board regulator),
- Common ground connections between supplies.

If separate relay power is not available, local decoupling helps:

- High capacity (e.g., ~1000µF) electrolytic near relay power input for low-frequency filtering. Relays draw large amounts of current and cause sag on the power rail. The electrolytic acts as a local battery, supplying that bulk current and keeping voltage stable.
- Low capacity (e.g., 100nF) ceramic close to each sensor module for high-frequency filtering. These catch the high-frequency switching noise and transients that come from relay coils and digital logic, preventing them from affecting the sensitive measurements.

## Pin Map

All control I/O is mapped to **digital pins** on the Mega. This should be identical to `include/config.h`.

### 1) Sensors

Wire each ultrasonic module to its own TRIG/ECHO pair:

- Track 1 sensor: `TRIG -> D22`, `ECHO -> D23`
- Track 2 sensor: `TRIG -> D24`, `ECHO -> D25`
- Track 3 sensor: `TRIG -> D26`, `ECHO -> D27`
- Track 4 sensor: `TRIG -> D28`, `ECHO -> D29`

### 2) Signal relays

Each signal has two relay control lines: Green coil command and Red coil command.

- Signal 1: `S1_G -> D2`, `S1_R -> D3`
- Signal 2: `S2_G -> D4`, `S2_R -> D5`
- Signal 3: `S3_G -> D6`, `S3_R -> D7`
- Signal 4: `S4_G -> D8`, `S4_R -> D9`

Why two lines per signal:

- These signal actuators are **pulse-driven**, not continuously powered,
- One short pulse on the **Green** line moves the mechanism to green, one short pulse on the **Red** line moves it back to red,
- Holding either line active continuously can overheat/damage coils or driver components, so firmware always uses timed pulses (`RELAY_PULSE_MS`) only.

### 3) Turnout / switch relays

- Switch left command: `SW_L -> D10`
- Switch right command: `SW_R -> D11`

### 4) Station request buttons (digital inputs)

Buttons are configured as `INPUT_PULLUP`, so the pressed state is logic LOW.

- Button 1 signal wire: one side of button to `D30`, other side to `GND`,
- Button 2 signal wire: one side of button to `D31`, other side to `GND`.

No external resistor is required with `INPUT_PULLUP`.

## Configuration Parameters

Tune values in `include/config.h`:

- `DETECT_ENTER_CM` < `DETECT_EXIT_CM`: occupancy thresholds,
- `SENSOR_SAMPLES`, `SENSOR_CONFIDENCE`: vote size and acceptance threshold,
- `TRACK_STATE_DWELL_MS`: minimum state hold time,
- `RELAY_PULSE_MS`: relay pulse width,
- `BLANKING_MS`, `MIN_BLANKING_MS`, `MAX_BLANKING_MS`, `BLANKING_STEP_MS`: blanking parameters,
- `SENSOR_INTERVAL`: sensor cycle period,
- `BUTTON_DEBOUNCE_MS`, `LOCKOUT_DURATION`: input stability and command pacing.

## Project Layout

`*` = simulation-only

```text
Little Innovators Train Network/
├── include/
│   ├── config.h
│   ├── relay_control.h
│   ├── signal_control.h
│   ├── sensor_control.h
│   ├── button_control.h
│   └── sim_signal_state.h *
├── src/
│   ├── main.cpp
│   ├── relay_control.cpp
│   ├── signal_control.cpp
│   ├── sensor_control.cpp
│   ├── button_control.cpp
│   └── sim/
│       ├── sim_main.cpp *
│       └── sim_signal_control.cpp *
├── diagram.json *
├── platformio.ini
├── wokwi.toml *
└── README.md
```
