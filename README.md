# NP1 DRONE
Building a quadcopter from scratch. Hardware and software.
## Hardware Overview
Hardware includes 3 custom PCB designs: 
1. Flight Control Computer (FCC)
2. Electronic Speed Controller (ESC)
3. Remote Controller (RC).
#### Flight Control Computer
The FCC PCB incorporates:
- STM32G474 microcontroller
- IMU (accelerometer + gyroscope), magnetometer, barometer
- USB 2.0 Full Speed
- 2 CANbus transceivers (one for ESC comms and another for serial payloads)
- 2.4GHz RF transceiver
- 32kBit EEPROM
- Power regulation (4S LIPO 16.8V max -> +5V -> +3.3V)
- Payload header (exposing +5V, +3.3V, GPIO, I2C, SPI, UART)
#### Electronic Speed Controller
The ESC PCB incorporates:
- STM32G474 microcontroller
- 3 Half-bridge MOSFET gate drivers
- 6 N-channel power MOSFETS (in 3 half-bridge topologies)
- Per phase voltage measurement
- Per phase current measurement
- Back-EMF zero-crossing detection (using MCU comparators)
- CANbus transceiver (comms to FCC)
- 32kBit EEPROM
- Power regulation (4S LIPO 16.8V max -> +5V -> +3.3V)
#### Remote Controller
The RC PCB has yet to be implemented. It will have RF communication with the FCC.

## Software Overview [Not Complete]
Each PCB (FCC, ESC, and RC) has a dedicated STM32 MCU running FreeRTOS (using CMSIS_V2 API).
#### Flight Control Computer
Will need:
- Kalman filter for state estimation
- Control loop (setpoint from remote controller -> PID -> commands to ESCs)
#### Electronic Speed Controller
The NP1 ESC hardware was designed to support the following BLDC control algorithms:
- Trapezoidal control (Back-EMF zero-crossing detection)
- Field-oriented control (Voltage and current measurements per phase)
#### Remote Controller
Needs to send messages to FCC (message definitions TBD)


