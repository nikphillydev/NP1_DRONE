# NP1 DRONE
Building a quadcopter from scratch. Hardware and software.
## Hardware Overview
Hardware includes 3 custom PCB designs: 
1. the Flight Control Computer (FCC)
2. Electronic Speed Controller (ESC)
3. Remote Controller (RC).
### FCC
The FCC PCB incorporates:
- STM32G474CET6
- IMU, magnetometer, barometer
- USB 2.0
- 2 CANbus transceivers (one for ESC comms and another for arbitrary payloads)
- 2.4GHz RF transceiver
- EEPROM
- Payload header (exposing +5V, +3.3V, GPIO, I2C, SPI, UART)
### ESC
The ESC PCB is a simple ESC design incorporating:
- STM32G431KBT6 microcontroller
- 3 Half-bridge MOSFET gate drivers
- 6 N-channel power MOSFETS (in 3 half-bridge topologies)
- Back EMF Zero Crossing detection using onboard comparators on the MCU
- CANbus tranceiver (comms to FCC)
## RC
The RC PCB has yet to be implemented. It will have RF communication with the FCC.



