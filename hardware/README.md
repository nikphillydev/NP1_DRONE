## Hardware Overview
This project includes 2 custom PCB designs: 
1. Flight Control Computer (FCC)
2. Electronic Speed Controller (ESC)

#### PCB: Flight Control Computer [COMPLETE]
The FCC PCB incorporates:
- STM32G474 microcontroller
- BMI088 IMU (accelerometer + gyroscope), LIS3MDL magnetometer, BMP388 barometer
- USB 2.0 Full Speed (Data + Power)
- 2 SN65HVD232 CANbus transceivers (one bus for ESC comms and another bus for serial payloads)
- CC2500 2.4GHz RF transceiver (for comms with ground station)
- M24C32 32kBit EEPROM
- Power regulation (4S / 6S LIPO -> +5V -> +3.3V)
- Payload header (exposing +5V, +3.3V, GPIO, I2C, SPI, UART)

#### PCB: Electronic Speed Controller [COMPLETE]
The ESC PCB incorporates:
- STM32G474 microcontroller
- 3 Half-bridge MOSFET gate drivers to support a 3 phase BLDC motor
- 6 N-channel power MOSFETS (in 3 half-bridge topologies)
- Per phase voltage measurement
- Back-EMF zero-crossing detection (using MCU comparators)
- CANbus transceiver (comms to FCC)
- Power regulation (4S / 6S LIPO -> +11V -> +5V -> +3.3V) 