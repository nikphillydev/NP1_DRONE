# NP1 DRONE
Building a quadcopter from scratch. Hardware and software.
## Hardware Overview
Hardware includes 3 custom PCB designs: 
1. Flight Control Computer (FCC)
2. Electronic Speed Controller (ESC)
3. Remote Controller (RC).
### Flight Control Computer
The FCC PCB incorporates:
- STM32G474 microcontroller
- IMU, magnetometer, barometer
- USB 2.0
- 2 CANbus transceivers (one for ESC comms and another for serial payloads)
- 2.4GHz RF transceiver
- EEPROM
- Power regulation (4S LIPO 16.8V max -> +5V -> +3.3V)
- Payload header (exposing +5V, +3.3V, GPIO, I2C, SPI, UART)
### Electronic Speed Controller
The ESC PCB is a simple ESC design incorporating:
- STM32G431 microcontroller
- 3 Half-bridge MOSFET gate drivers
- 6 N-channel power MOSFETS (in 3 half-bridge topologies)
- Back EMF Zero Crossing detection using onboard comparators on the MCU
- CANbus transceiver (comms to FCC)
- Power regulation (4S LIPO 16.8V max -> +5V -> +3.3V)
## Remote Controller
The RC PCB has yet to be implemented. It will have RF communication with the FCC.



