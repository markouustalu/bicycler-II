# LCD - Controller Protocol NO2 Communication Protocol (Version 20161020)

## Revision History
1. Speed limit value was originally defined as two bytes, but the high byte was always zero. For special LCD's, this high byte is redefined as "speed limit dedicated motor pole pair count"
2. The number of PAS disc magnet count occupies one byte, but only half a byte is needed. The high half-byte is used for specific parameters:
   - Bit7: Reverse control
   - Bit6: Automatic cruise control switch
3. In the control transmission section, Bit1 and Bit0 of "Controller Status 2" are added to indicate the PAS level (when mobile connects to controller, LCD only displays PAS level)
   - Bit5 of "Controller Status 2" is added as brake to distinguish from brake fault (Bit1 of "Controller Status 1")

## Speed Measurement Methods
1. Motor phase voltage can detect motor speed, but due to multiple defects, this method is not used in this protocol (exceptionally required customers can connect directly, not through protocol)
2. Hall sensor speed measurement using motor pole pairs:
   - With 30+ motor pole pairs, this method provides many signals and fast response
   - However, export high-speed motors often have clutches, making Hall sensors unable to detect speed during coasting. This method has defects and is not commonly used in export vehicles (which require coasting capability)
   - In skateboards and similar vehicles without clutches, Hall sensors are commonly used for speed measurement
3. To solve Hall sensor issues, some motors have built-in Hall sensors unaffected by clutches (1-6 signals), or wheel-mounted Hall sensors (1 signal)
4. The protocol supports both motor pole pair measurement and independent Hall speed measurement, determined by parameter value (See 1.4 for more information):
   - When parameter ≤ 6: independent Hall speed measurement
   - When parameter > 6: motor pole pair speed measurement
5. There are 3 definitions regarding motor pole pair in this protocol:
   - Speed measurement motor pole pair: (see 1.4 for specific definition) Used for "speed feedback" calculation and "speed limit" calculation. Generally, controllers only need to implement this parameter.
   - Speed limit dedicated motor pole pair count: (see 1.8 for specific definition) In most cases, controllers can use speed measurement motor pole pair for speed limit calculation. However, when speed measurement motor pole pair count is 1 or very few, the speed signal acquisition time is too long, especially when the controller's speed control is closed-loop, the effect is not ideal (open-loop is not a problem). Therefore, some controller manufacturers require the LCD to provide a configurable speed limit dedicated motor pole pair count for customers to set. This function is only provided when controller manufacturers specifically request it, or it is a special function that most controller manufacturers can ignore.

## Basic Data Format

| Address (1 byte) | Frame Length (1 byte) | Command Number (1 byte) | Data Area (N bytes) | Checksum (1 byte) |
|----------------|------------------------|----------------------|-------------------|------------------|
| 0x01           | Length of the entire frame (4+N bytes) | Data type identifier | Variable length data area | Checksum calculated using the following algorithm |

### Checksum Algorithm
For received data: 0x01 0x03 0x02 0x00 0x03 0x00 0x00 0x00 0x00 0x81 0x00 0x00 0x00 0x00 0x82
Checksum = 0x01 ^ 0x03 ^ 0x02 ^ 0x00 ^ 0x03 ^ 0x00 ^ 0x00 ^ 0x00 ^ 0x00 ^ 0x81 ^ 0x00 ^ 0x00 ^ 0x00 ^ 0x00 ^ 0x82 = 0

### Frame Segmentation
Frames are separated by idle time. Receiver should respond with response after approximately 10-20ms. Transmission timeout to detect end of transmission should be no less than 50ms.

### Multi-byte Definition
Multi-byte data: high byte first, then low byte

### Serial Port Definition
UART transmission, 9600 baud, 8 data bits, 1 stop bit, no parity check

### Frame Length
Frame length is only for verification purposes (see examples). When protocol upgrades and adds length, controllers can maintain backward compatibility.

**Sample Code:**
```c
// Compatible approach - check actual frame length against expected
if(recData[1] != recCount) { 
    return; 
}

// Incompatible approach - hard-coded length check
if(recCount != 20) { 
    return; 
}

// Flexible length range check
if(!( (recCount >= 20) && (recCount <= 25))) { 
    return; 
}
```

## LCD to Controller Communication (Version 2 Protocol) - 20-byte format

### Frame Structure

| Field Name | Size (bytes) | Description |
|------------|--------------|-----------|
| Address | 1 | 0x01 |
| Frame Length | 1 | 20 |
| Command Number | 1 | 0x01 |
| Drive Mode Setting | 1 | 1~3 |
| PAS Level Setting | 1 | 0~15 (see below for details) |
| Controller Setting | 1 | see below for details |
| Speed motor pole pair Count | 1 | see below for details |
| Wheel Diameter | 2 | Unit: 0.1 inch |
| PAS Sensitivity | 1 | see below for details |
| PAS Start Intensity | 1 | see below for details |
| Speed Limit Dedicated Motor pole pair Count + Speed Limit Value | 2 | Originally 2 byte speed limit value (see below for details) |
| Controller Current Limit | 1 |  |
| Controller Undervoltage Value | 2 | see below for details |
| Throttle Speed PWM Duty Cycle | 2 | see below for details |
| Controller Setting 2 + PAS Disc Type Selection | 1 | High 4 bits: Controller Setting 2, Low 4 bits: PAS Disc Type Selection (see below for details) |
| Checksum | 1 | See checksum algorithm |

### 1.1 Drive Mode Setting
- 0: PAS Assistance drive (output power and speed is based on PAS assistance level, twist throttle is ignored)
- 1: Twist throttle drive (twist throttle control, PAS assistance level is ignored)
- 2: Both PAS assistance and twist throttle drive (twist throttle drive invalid at zero start)

### 1.2 PAS Assistance Level

| Level* | PAS Assistance Type        | PAS Levels                                                                 |
|-----|------------------------|-------------------------------------------------------------------------------|
| 0   | 3-level PAS assistance     | Levels 5, 10, 15 for 1st, 2nd, 3rd PAS                                 |
| 1   | 5-level PAS assistance | Levels 3, 6, 9, 12, 15 for 1st-5th PAS                               |
| 2   | 9-level PAS assistance | Levels 1, 3, 5, 7, 9, 11, 13, 14, 15 for 1st-9th PAS                   |
| Bit7| Reverse PAS           | 0 for normal PAS, 1 for reverse PAS (0x81 for reverse 1st PAS, 0x82 for reverse 2nd PAS, etc.) |

\* On some LCDs levels 0,1,2 have been changed to 3,5,9 to display how many levels will be available

### 1.3 Controller Control Setting 1
| Bit | Name                     | Value Range     | Comment                                                                 |
|-----|--------------------------|-----------------|-------------------------------------------------------------------------|
| 7   | Controller switch status | 0: off, 1: on |                                                                         |
| 6   | Zero start             | 0: zero start, 1: non-zero start |                                                         |
| 5   | Light control          | 0: off, 1: on   |                                                                         |
| 4   | Communication fault    | 0: normal, 1: fault | (cannot receive data from the controller)                                   |
| 3   | Cruise switching method | 0: trigger mode, 1: long press mode | (0: LCD default state)                                           |
| 2   | Speed limit status     | 1: over limit, 0: under limit | (no speed limit is always 0)                                        |
| 1   | Cruise hold mode       | 0: not cruising, 1: cruising | (If you want to hold and release the cruise control such as "6KM/Boost", the controller can directly read this location) |
| 0   | Cruise switching mode  | 0: No need to switch, 1: Need to switch cruise | (If you need to hold down the cruise control to reverse the state, such as fixed speed cruise control, you can directly read this position) |

### 1.4 Motor pole count (not pole pair count)
Determined by the number of motor pole count
1. Inner speed measurement (motor pole count) if number of magnets is >6
2. Outer speed measurement (independent Hall signal on motor wire or fixed on vehicle) if 6 or fewer motor pole counts
Common: 1 motor pole (wheel-mounted sensor) or >6 motor pole (motor internal, 6-wire motor)
Example: Most motors have 46 motor pole

### 1.5 Wheel Diameter
Unit: 0.1 inches
Example: 305 = 30.5 inches

### 1.6 PAS Assistance Sensitivity
Number of pulses that need to be detected before PAS assistance starts: (3-24)

### 1.7 Assistance Start Intensity
Relative PWM signal strength at PAS assistance start (0-5, 0 weakest, 5 strongest)

### 1.8 Speed Limit Dedicated Motor pole pair Count + Speed Limit Value
The speed limit function is relatively complex. If customers do not require this speed limit feature, it can be considered for non-implementation. In the early protocol, these 2 bytes were both used for speed limit values, but since actual speed limits never exceed 255 km/h, the high byte was always 0 and unused. Therefore, the high byte has now been repurposed to indicate the inner speed measurement motor pole pair count.

This change provides better precision for closed-loop speed control. While external speed measurement motor pole pair count generally does not exceed 6 (usually 1), achieving smooth control with just one motor pole pair is difficult. By providing the internal motor pole pair count on the LCD panel, more precise data becomes available, making it easier for controllers to implement closed-loop speed limiting. Although some controllers can directly match with motors and know the motor pole pair count, placing this setting on the LCD panel offers greater flexibility.

New controller programs should only read the low byte (ignoring the high byte), as only specific LCDs will send motor pole pair count in the high byte. Since old controllers are unlikely to be mass-produced with such LCDs, compatibility issues are minimal.

### 1.9 Controller Current Limit
Unit: 1 Ampere, the maximum operating current of the controller (default is 12)

### 1.10 Controller Undervoltage Value
Unit: 0.1V

### 1.11 Throttle Speed PWM Duty Cycle
Controller output PWM duty cycle, unit: 0.1% (max 1000, default 0)
The throttle speed PWM duty cycle controls the controller output, with units in 0.1% increments (where 1000 represents 100%). For safety, it is recommended that the controller implement a gradual braking sequence to come to a complete stop if valid communication data is not received within a specified time period (e.g., 2 seconds). This is particularly important as the controller sends data approximately every 100-150ms.

### 1.12 Controller Setting 2 + PAS Assistance Disc magnet Count
| Bit | Name                     | Description                                                                 |
|-----|--------------------------|-----------------------------------------------------------------------------|
| 7   | Reverse flag             | 0: invalid, 1: reverse                                                     |
| 6   | Automatic cruise flag    | 0: no auto cruise, 1: auto cruise                                          |
| 5   | Reserved               | Reserved                                                                 |
| 4   | Reserved               | Reserved                                                                 |
| 3-0 | PAS assistance disc magnet count | 5,8,12 magnets |

**Note:** 
- The original protocol had this byte's high 4 bits as 0.
- After the correction, some special function LCDs may send "reverse" data, but this won't affect regular controllers.
- New controllers filter out the upper bits when reading "PAS assistance disc magnet count" (&0x0F) to ensure full compatibility.

## Controller to LCD Communication

### Command 1 Data Frame (14-byte format)

| Field                 | Size (bytes) | Description                                              |
|----------------------|------------|----------------------------------------------------------|
| Address            | 1          | 0x02                                                     |
| Frame Length       | 1          | 14                                                       |
| Command Number     | 1          | 0x01                                                     |
| Controller Status 1    | 1          | See below for details                                  |
| Controller Status 2    | 1          | See below for details                                  |
| Running Current    | 2          | Unit 1A/0.1A                                             |
| Current Ratio Value| 1          | See below for details                                  |
| Speed Feedback   | 2          | See below for details                                  |
| Battery Capacity   | 1          | 0~100 Percent                                          |
| Remaining Mileage  | 2          | Unit: km                                               |
| Checksum           | 1          | See checksum algorithm                                 |

### 2.1 Controller Status 1

| Bit | Status Description                     | Value |
|-----|--------------------------------------|-------|
| 7   | 6km cruise status                  | 1: in cruise, 0: not in cruise |
| 6   | Hall fault status                  | 1: fault, 0: no fault |
| 5   | Throttle fault status              | 1: fault, 0: no fault |
| 4   | Controller fault status            | 1: fault, 0: no fault |
| 3   | Undervoltage protection status       | 1: in protection, 0: not in protection |
| 2   | Cruise status                      | 1: in cruise, 0: not in cruise |
| 1   | Brake fault                        | 1: fault, 0: no fault |
| 0   | Motor phase loss fault             | 1: fault, 0: no fault |

### 2.2 Controller Status 2

| Bit | Name                     | Description                                                                 | Default Value         |
|-----|--------------------------|---------------------------------------------------------------------------|-----------------------|
| 7   | Vehicle level status     | 1: level, 0: not level                                                   | 1 (if not implemented)  |
| 6   | PAS Assistance sensor status | 1: fault, 0: normal                                                    | 1 (when not available)|
| 5   | Power-off brake        | 1: brake active, 0: brake inactive                                        | -                     |
| 4   | Communication fault    | 1: fault, 0: normal                 | (cannot receive info from the LCD)                     |
| 3   | Charging status        | 1: charging, 0: not charging                                                | -                     |
| 2   | Speed limit status   | 0: not limited, 1: limited                                                 | -                     |
| 1-0 | PAS level             | 0-3                   | (PAS position not controlled by LCD in some cases)                     |

\* The difference between BIT1 in controller state 1 (brake fault) and BIT5 in state 2 is that one is fault detection and the other is status detection

### 2.3 Running Current (2 bytes)
- High byte bit 6 determines unit: 1 = 0.1A, 0 = 1A
- Example: 0x0010 = 16A, 0x4010 = 1.6A

The running current is represented by 2 bytes. In earlier protocols, the default unit was 1A. However, considering practical needs, 0.1A is more suitable. Therefore, the unit is modified as follows:
- When bit 6 of the high byte is 1, the unit is 0.1A
- When bit 6 of the high byte is 0, the unit remains 1A
- Example: 0x0010 = 16A, 0x4010 = 1.6A
- Bit 7 of the high byte is reserved for positive/negative values (currently kept as 0)

### 2.4 Current Ratio Value
1-100% current ratio value (compatible with 36V, 48V, 100% = current limit)
On some controllers this is used as the motor temperature, which is represented in C and offset by 40. So 0 value = -40C.

### 2.5 Speed Feedback
- Unit: ms per wheel revolution
- For motor pole pair count ≤ 6: one Hall pulse = one revolution
- For motor pole pair count > 6: half Hall pulses = one revolution
- When motor stops: 3.5s timeout for reasonable balance between response time and minimum speed display

### 2.6 Battery Capacity and Remaining Mileage
- Implemented by BMS (if no BMS, send 0)