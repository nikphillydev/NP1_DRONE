# NP1 DRONE
Building a quadcopter from scratch. Hardware (KiCAD) and software (STM32 C/C++) by Nikolai Philipenko, University of Alberta.
## Hardware Overview [In Progress]
Hardware includes 2 custom PCB designs on the vehicle side: 
1. Flight Control Computer (FCC)
2. Electronic Speed Controller (ESC)

And a RaspberryPi 5 with a 2.4GHz RF transceiver running the ground station software and commanding the drone.
#### Flight Control Computer [COMPLETE]
The FCC PCB incorporates:
- STM32G474 microcontroller
- IMU (accelerometer + gyroscope), magnetometer, barometer
- USB 2.0 Full Speed
- 2 CANbus transceivers (one for ESC comms and another for serial payloads)
- 2.4GHz RF transceiver
- 32kBit EEPROM
- Power regulation (4S LIPO 16.8V max -> +5V -> +3.3V)
- Payload header (exposing +5V, +3.3V, GPIO, I2C, SPI, UART)
#### Electronic Speed Controller [SCHEMATIC PHASE]
The ESC PCB incorporates:
- STM32G474 microcontroller
- 3 Half-bridge MOSFET gate drivers to support a 3 phase BLDC motor
- 6 N-channel power MOSFETS (in 3 half-bridge topologies)
- Per phase voltage measurement
- Per phase current measurement
- Back-EMF zero-crossing detection (using MCU comparators)
- CANbus transceiver (comms to FCC)
- 32kBit EEPROM
- Power regulation (4S LIPO 16.8V max -> +5V -> +3.3V)

## Software Overview [In Progress]
Each custom vehicle PCB (FCC and ESC) has a dedicated STM32 MCU running FreeRTOS (using CMSIS_V2 API). The ground station software runs on the RPi5 and is built on PyQT6.
#### Flight Control Computer
The flight control computer implements software drivers for the onboard accelerometer, gyroscope, magnetometer, and barometer. Using this data, the FCC implements
- Calibration for all sensors
- 9-axis + barometer extended kalman filter (EKF) for state estimation [x, y, z, roll, pitch, yaw]

The FCC also communicates with ground station for the following control modes:
  - Hover at specific altitude
  - Circular loiter at altitude and radius
  - Manual control 

All control modes use a PID controller that converts thrust to RPM commands that are sent to the 4 ESCs via CANbus.
#### Electronic Speed Controller
The NP1 ESC hardware was designed to support the following BLDC control algorithms:
- Trapezoidal control (Back-EMF zero-crossing detection)
  - This will be implemented first
- Field-oriented control (Voltage and current measurements per phase)
#### NP1 Ground Control Station (GCS)
The NP1 GCS displays current sensor and position data as well as information, critical, and error logs from the FCC.


