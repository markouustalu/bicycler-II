# Bicycler-II

So, i fried my LCD that came with my SVMC72200 when i used regen braking and BMS cut off the battery because it was absolutely full. The resulting voltage spike was too much and completely burned out the H6 display. New one was 100€+, so i did what every normal tinkerer does, i thought, how hard can it be, i'll make one myself. So, countless hours later i have a working PlatformIO-based Arduino project for an e-bike controller display, implementing the NO2 communication protocol for LCD-controller interaction.

Luckily i was able to get the NO2 protocol description for the Sabvoton SVMC controllers. I have the V1 version of the controller. Much of the protocol is sadly not implemented by the controller.

## Screen

The screen has pretty much everything put on the single view. Nothing else is implemented, no menus or anything. The only thing is the "unlimited mode". During the first 5 seconds after boot you have to click the up button three times to remove PAS lvl 5 and 25kmh limit. This can only be activated and not deactivated. Deactivation is done using power cycle. If you miss the 5s window, the only option to try again is to do a power cycle.

### Screen explanation
![Sample screen](https://raw.githubusercontent.com/markouustalu/bicycler-II/refs/heads/main/display.jpg)

1. Speed in km/h (not configurable, change programming if needed)
2. PAS lvl and motor temp in C (not configrable, change programming if needed)
3. Wattage. Voltage is hard coded in at 52V. Change or adapt if needed.
4. Avg speed in kmh since boot and used Ah since boot
5. Time since last boot
6. Odometer (stored in flash) and trip meter (zeroed every boot)

## Features

- **NO2 Protocol Communication:** Exchanged data packets with Sabvoton SVMC controllers (see `docs/NO2 Protocol.md`).
- **Telemetry & Real-Time Dashboard:** Shows speed (km/h), pedal assist (PAS) level, motor temperature (°C), live wattage (calculated using a hardcoded 52V), uptime, trip distance, and odometer.
- **Regenerative Braking Integration:** Accumulates and drains Ampere-hours (Ah) dynamically based on running current; when regenerative braking is active (`STATUS2_BRAKE_ACTIVE`), it subtracts the regenerated power from the cumulative usage.
- **EEPROM Wear-Leveling Config:** Prevents wearing out the Arduino Nano's EEPROM cell. Saves trip statistics (odometer, cumulative Ah, trip distance, and elapsed time) in a circular wear-leveling array of 10 slots.
- **Display Timeout & Power Saving:** Dimming feature that sets OLED contrast to 0 to prevent burn-in if there is no serial activity for 5 seconds, returning to full contrast (255) when activity resumes.
- **Modular Code Structure:** Easily extensible firmware split cleanly into Display, Buttons, Config, and Protocol layers.

## Interactive Controls (Button Setup)

The display uses 3 buttons (Up, Down, Menu/M) via the `Button2` library, offering the following controls:

| Button Action | Function | Details |
| --- | --- | --- |
| **Up (Single Click)** | Increase PAS Level | Increments the assist level set point sent to the controller. |
| **Down (Single Click)** | Decrease PAS Level | Decrements the assist level set point sent to the controller. |
| **Up (Triple Click)** | Activate **Unlimited Mode** | Only active during the first **5 seconds** after power-on. Unlocks maximum assist levels up to 15, selects electric drive mode, and bypasses the 25km/h speed limit. To deactivate, power cycle the e-bike. |
| **Menu (Double Click)** | Reset Trip Stats | Resets session elapsed time, trip distance, and cumulative Ah used to zero. |
| **Menu (Triple Click)** | Manual EEPROM Save | Forces an immediate write of current session statistics (odometer, cumulative Ah, trip meter, session time) to EEPROM wear-leveling storage. |

## Hardware

- **Arduino Nano** (ATmega328p)
- **OLED Display** (SSD1309 128x64 display configured in 90-degree portrait rotation, using the robust `U8g2` library in full buffer mode)
- **3x Tactile Buttons** (Up, Down, Menu/M)
- **Serial Interface** (SoftwareSerial on pins 7 (Rx) and 9 (Tx) configured at 9600 baud for controller communication)

## Directory Structure

- `src/` - Main source code (protocol, display, buttons, config)
- `include/` - Header files
- `lib/` - (Optional) Private libraries
- `docs/` - Protocol documentation
- `test/` - Unit tests (PlatformIO Test Runner)

## Libraries Used

- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Button2](https://github.com/LennartHennigs/Button2)
- [U8g2](https://github.com/olikraus/u8g2)

## Getting Started

1. Install [PlatformIO](https://platformio.org/)
2. Clone this repository
3. Connect your Arduino Nano and required hardware
4. Build and upload using PlatformIO

## Configuration

Edit `platformio.ini` to select your board and adjust library versions as needed.

## Protocol

See `docs/NO2 Protocol.md` for details on the LCD-controller communication protocol.

## Author

Marko

## License

MIT License. Free to use and modify.
