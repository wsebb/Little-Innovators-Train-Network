# Railway Signaling System

Automated railway signaling control system with train detection and safety interlocking

Made for Little Innovators (Siemens)

## Quick Start

### PlatformIO (Recommended)

```bash
# Clone repository
git clone `<to add>`
cd RailwaySignaling

# Build and upload
pio run --target upload

# Monitor serial output
pio device monitor
```

### Arduino IDE

1. Download and extract project
2. Rename `src/main.cpp` to `<to add>.ino`
3. Open in Arduino IDE
4. Select Board: **Arduino Mega 2560**
5. Upload and open Serial Monitor (9600 baud)

## Simulation (No Hardware)

You can simulate this project in VS Code using **Wokwi** without connecting an Arduino.

### Setup

1. Install the VS Code extension: **Wokwi Simulator**
2. Build the firmware in this project:

```bash
pio run -e megaatmega2560_sim
```

3. Start simulation from VS Code command palette:
   - `Wokwi: Start Simulation`

### Included simulation files

- `wokwi.toml` links Wokwi to PlatformIO build artifacts (`firmware.hex` and `firmware.elf`)
- `diagram.json` contains a virtual Arduino Mega with:
  - 4x HC-SR04 ultrasonic sensors (pins 22-29)
  - 2x push buttons (pins 30-31, using `INPUT_PULLUP`)
  - 10x LEDs representing active-low relay outputs (pins 2-11)
  - 8x additional latched LEDs for signal state visualization (pins 32-39)

### Simulation-only code separation

- Production firmware remains in `src/main.cpp` and `src/signal_control.cpp`
- Simulation-specific logic is isolated in:
  - `src/sim/sim_main.cpp`
  - `src/sim/sim_signal_control.cpp`
  - `include/sim_signal_state.h`

### Notes

- Relay outputs are represented by LEDs (for visibility in simulation).
- Because the project uses active-low relays, LEDs are wired to turn ON when output pin is LOW.
- Wokwi validates logic flow and sequencing well, but EMI/power-noise behavior from real relay hardware is not fully reproduced.

## Pin Configuration

### Sensors
- Track 1: Trig=22, Echo=23
- Track 2: Trig=24, Echo=25
- Track 3: Trig=26, Echo=27 (Protection Zone)
- Track 4: Trig=28, Echo=29

### Signals (Green/Red)
- Signal 1: Pins 2/3
- Signal 2: Pins 4/5
- Signal 3: Pins 6/7
- Signal 4: Pins 8/9

### Controls
- Switch: Left=10, Right=11
- Button 1: Pin 30
- Button 2: Pin 31

## Power Supply Configuration

**Current Setup:** System runs on Arduino 5V output.

**Problem:** Relay switching creates electrical noise that interferes with sensors. The code includes EMI blanking to mitigate this, but hardware separation is better.

**Recommended Solution:**
- Use a **separate 5V power supply** for the relays (2A minimum)
- Keep Arduino & sensors on USB/Arduino power
- Connect grounds together

**Alternative:**
If separate supply unavailable, add capacitors to reduce noise:
- **1000µF electrolytic capacitor** near relay modules (smooths voltage drops when relays switch)
- **100nF ceramic capacitor** near each sensor (filters high-frequency switching noise)


## System Logic

**Track 1 & 2 (Station):**
- Signal turns RED when occupied
- Manual GREEN request allowed when Track 3 is clear

**Track 3 (Protection Zone):**
- Forces both station signals RED when occupied
- Prevents conflicting movements

**Track 4 (Main Line):**
- Automatic RED when occupied, GREEN when clear
- Controls switch positioning

**Station Capacity:**
- Signal 4 shows station availability
- RED when both platforms occupied

## Configuration

Edit `include/config.h` to tune parameters:

```cpp
const int DETECTION_THRESHOLD = 5;       // Detection distance (cm)
const int SENSOR_SAMPLES = 5;            // Readings per check
const int SENSOR_CONFIDENCE = 3;         // Min valid readings
const unsigned long RELAY_PULSE_MS = 200; // Relay pulse duration
const unsigned long BLANKING_MS = 500;    // EMI blanking period
```

## Safety Features

- **EMI Blanking**: 500ms sensor pause after relay activation
- **Confidence Detection**: Multi-sample validation reduces false triggers
- **Protection Zones**: Track 3 acts as safety interlock
- **Lockout Timers**: Prevents rapid state changes
- **State Validation**: All manual requests checked against system state

## Project Structure

`*` = SIM only

```
RailwaySignaling/
├── diagram.json            # Wokwi circuit diagram *
├── platformio.ini          # PlatformIO configuration
├── wokwi.toml              # Wokwi simulation config *
├── include/
│   ├── config.h           # Pin definitions & parameters
│   ├── relay_control.h    # Relay management
│   ├── signal_control.h   # Signal/switch control
│   ├── sensor_control.h   # Sensor & track logic
│   ├── button_control.h   # User input handling
│   └── sim_signal_state.h # Simulation signal state helpers *
├── src/
│   ├── main.cpp           # Main program & initialization
│   ├── relay_control.cpp  # Relay timing functions
│   ├── signal_control.cpp # Signal/switch functions
│   ├── sensor_control.cpp # Sensor processing
│   ├── button_control.cpp # Button handling
│   └── sim/
│       ├── sim_main.cpp           # Simulation entry point *
│       └── sim_signal_control.cpp # Simulation signal logic *
└── README.md
```

## Serial Monitor Output

The system provides real-time status updates:

```
=========================================
  Railway Signaling System v2.1
  Active-Low Relay Logic
  EMI Blanking: 500ms
=========================================

-> [1/5] Setting Signal 1 to RED
-> [2/5] Setting Signal 2 to RED
-> [3/5] Setting Signal 3 to GREEN
-> [4/5] Setting Signal 4 to GREEN
-> [5/5] Setting Switch to LEFT

System operational - Starting sensor monitoring

[SENSOR] Scanning sensors...
  Track Status: T1=CLR T2=CLR T3=CLR T4=CLR
[TRACK 1] OCCUPIED
  [SIGNAL] 1 -> RED
  [RELAY] Pin 3 activated
```
