# Morse Code Text Generator

Arduino-based Morse code input device with LCD display.

## Hardware Required
- Arduino Uno
- 16x2 LCD Display
- 10kΩ Potentiometer (contrast)
- 2 Push Buttons
- Breadboard & jumper wires

## Wiring

### LCD to Arduino
```
VSS → GND    |  RS → Pin 12   |  D4 → Pin 5
VDD → 5V     |  EN → Pin 11   |  D5 → Pin 4
V0 → Pot     |                |  D6 → Pin 3
                              |  D7 → Pin 2
```

### Buttons to Arduino
```
Morse Input Button: Pin 7 → Button → GND
Display Button:     Pin 8 → Button → GND
```

### Potentiometer
```
Pin 1 → 5V
Pin 2 (center) → LCD V0
Pin 3 → GND
```

## Setup
**Arduino IDE:** Upload the provided code
**PlatformIO:** Add to `platformio.ini`:
```ini
lib_deps = arduino-libraries/LiquidCrystal@^1.0.7
```

## Usage
1. **Input Morse:** Press button short (dot) or long (dash)
2. **Complete letter:** Wait 1 second after last input
3. **Display text:** Press display button

### Timing
- **Dot:** < 200ms press
- **Dash:** 200-600ms press  
- **Letter gap:** 1 second pause

### Common Letters
```
A: .-    E: .     H: ....   L: .-..   O: ---
```

## Troubleshooting
- **Blank LCD:** Adjust potentiometer contrast
- **No response:** Check button wiring to GND
- **Debug:** Open Serial Monitor (9600 baud)

---
*Short press = dot, long press = dash, wait to complete letters*
