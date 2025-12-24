# NP1 DRONE
Building a quadcopter from scratch. Hardware (KiCAD) and software (STM32 C/C++) by Nikolai Philipenko, University of Alberta.
## Hardware Overview
This project includes 2 custom PCB designs on the vehicle side: 
1. Flight Control Computer (FCC)
2. Electronic Speed Controller (ESC)

And a RaspberryPi 5 with a 2.4GHz RF transceiver running the ground station software and commanding the drone.
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

## Software Overview
Each custom vehicle PCB (FCC and ESC) has a dedicated STM32 MCU running FreeRTOS (using CMSIS_V2 API). The ground station software runs on the RPi5 and is built on PyQT6.
### Software: Flight Control Computer [WIP]
The flight control computer software performs a multitude of functions involving vehicle command and control:

**Sensor fusion:** Uses custom software drivers to get data from the on-board accelerometer, gyroscope, magnetometer, and barometer, and the off-board PMW3901 optical flow sensor and US100 range finder. Using this data, the FCC implements
- 9-axis extended kalman filter (EKF) for orientation estimation [roll, pitch, yaw]
- Velocity complimentary filter fusing velocities integrated from accelerometer data and velocites from the optical flow camera and range finder
- Altitude complimentary filter fusing altitude from range finder (transformed from body frame to world frame) and altitude from the onboard barometer

**Over-the-air (OTA) Comms with GCS:** Not yet complete

**Control system:** Not yet complete. All control modes use a PID controller that converts thrust to RPM commands that are sent to the 4 ESCs via CANbus. Will support the following control modes:
  - Hover at specific altitude
  - Circular loiter at altitude and radius
  - Manual control 

### Software: Electronic Speed Controller  [N/A]
The NP1 ESC hardware was designed to support the following BLDC control algorithms:
- Trapezoidal control (Back-EMF zero-crossing detection)
  - This will be implemented first
- Field-oriented control (Voltage and current measurements per phase)
### Software: NP1 Ground Control Station (GCS)  [WIP]
The NP1 GCS displays current vehicle state as well as logging information from the FCC.


