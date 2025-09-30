# Bicycler-II

So, i fried my LCD that came with my SVMC72200 when i used regen braking and BMS cut off the battery because it was absolutely full. The resulting voltage spike was too much and completely burned out the H6 display. New one was 100€+, so i did what every normal tinkerer does, i thought, how hard can it be, i'll make one myself. So, countless hours later i have a working PlatformIO-based Arduino project for an e-bike controller display, implementing the NO2 communication protocol for LCD-controller interaction.

Luckily i was able to get the NO2 protocol description for the Sabvoton SVMC controllers. I have the V1 version of the controller. Much of the protocol is sadly not implemented by the controller.

## Screen

The screen has pretty much everything put on the single view. Nothing else is implemented, no menus or anything. The only thing is the "unlimited mode". During the first 5 seconds after boot you have to click the up button three times to remove PAS lvl 5 and 25kmh limit. This can only be activated and not deactivated. Deactivation is done using power cycle. If you miss the 5s window, the only option to try again is to do a power cycle.

### Screen explanation
![Sample screen](TBD)

1. Speed in km/h (not configurable, change programming if needed)
2. PAS lvl and motor temp in C (not configrable, change programming if needed)
3. Wattage. Voltage is hard coded in at 50V. Change or adapt if needed.
4. Avg speed in kmh since boot and used Ah since boot
5. Time since last boot
6. Odometer (stored in flash) and trip meter (zeroed every boot)

## Features

- Communicates with e-bike controller using the NO2 protocol (see `docs/NO2 Protocol.md`)
- Displays real-time data (speed, current, PAS level, etc.) on an OLED screen
- Button interface for user input (PAS level, unlimited mode, etc.)
- Persistent configuration storage (wheel diameter, odometer, etc.)
- Modular code structure for easy extension

## Hardware

- Arduino Nano (ATmega328)
- OLED display (SSD1306/SSD1309, via U8g2/Adafruit libraries)
- Buttons (Up, Down, Menu)
- Serial connection to e-bike controller

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
