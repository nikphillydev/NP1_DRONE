from collections import deque

class SensorData():
    """ This class holds data samples of various sensors.
    """
    def __init__(self, max_samples: int) -> None:
        self.acc_samples = BMI088_AccelerometerSamples(max_samples)
        self.gyro_samples = BMI088_GyroscopeSamples(max_samples)
        self.bar_samples = BMP388_BarometerSamples(max_samples)
        self.mag_samples = LIS3MDL_MagnetometerSamples(max_samples)
        self.state = Vehicle_State()
        
    def add_accelerometer_sample(self, x: float, y: float, z: float, temp: float, tp: float) -> None:
        self.acc_samples.add_sample(x, y, z, temp, tp)
    
    def add_gyroscope_sample(self, x: float, y: float, z: float, tp: float) -> None:
        self.gyro_samples.add_sample(x, y, z, tp)
    
    def add_barometer_sample(self, pressure: float, altitude: float, temperature: float, tp: float) -> None:
        self.bar_samples.add_sample(pressure, altitude, temperature, tp)
    
    def add_magnetometer_sample(self, intensity_x: float, intensity_y: float, intensity_z: float, tp: float) -> None:
        self.mag_samples.add_sample(intensity_x, intensity_y, intensity_z, tp)
    
    def update_state(self, roll: float, pitch: float, yaw: float) -> None:
        self.state.update(roll, pitch, yaw)
   
class Vehicle_State():
    """ This class holds information about the current vehicle state.
    """
    def __init__(self) -> None:
        self.roll = 0
        self.pitch = 0
        self.yaw = 0
    
    def update(self, roll: float, pitch: float, yaw: float) -> None:
        self.roll = roll
        self.pitch = pitch
        self.yaw = yaw
    
class BMI088_AccelerometerSamples():
    """ This class holds BMI088 accelerometer data samples.
    """
    def __init__(self, max_samples: int) -> None:
        self.acc_x_data = deque(maxlen=max_samples)
        self.acc_y_data = deque(maxlen=max_samples)
        self.acc_z_data = deque(maxlen=max_samples)
        self.acc_temp_data = deque(maxlen=max_samples)
        self.tp_data = deque(maxlen=max_samples)
    
    def add_sample(self, x: float, y: float, z: float, temp: float, tp: float) -> None:
        self.acc_x_data.append(x)
        self.acc_y_data.append(y)
        self.acc_z_data.append(z)
        self.acc_temp_data.append(temp)
        self.tp_data.append(tp)
        
        
class BMI088_GyroscopeSamples():
    """ This class holds BMI088 gyroscope data samples.
    """
    def __init__(self, max_samples: int) -> None:
        self.rate_x_data = deque(maxlen=max_samples)
        self.rate_y_data = deque(maxlen=max_samples)
        self.rate_z_data = deque(maxlen=max_samples)
        self.tp_data = deque(maxlen=max_samples)
    
    def add_sample(self, x: float, y: float, z: float, tp: float) -> None:
        self.rate_x_data.append(x)
        self.rate_y_data.append(y)
        self.rate_z_data.append(z)
        self.tp_data.append(tp)
        
        
class BMP388_BarometerSamples():
    """ This class holds BMP388 barometer data samples.
    """
    def __init__(self, max_samples: int) -> None:
        self.press_data = deque(maxlen=max_samples)
        self.altitude_data = deque(maxlen=max_samples)
        self.temp_data = deque(maxlen=max_samples)
        self.tp_data = deque(maxlen=max_samples)
    
    def add_sample(self, pressure: float, altitude: float, temperature: float, tp: float) -> None:
        self.press_data.append(pressure)
        self.altitude_data.append(altitude)
        self.temp_data.append(temperature)
        self.tp_data.append(tp)
        
        
class LIS3MDL_MagnetometerSamples():
    """ This class holds LIS3MDL magnetometer data samples.
    """
    def __init__(self, max_samples: int) -> None:
        self.intensity_x = deque(maxlen=max_samples)
        self.intensity_y = deque(maxlen=max_samples)
        self.intensity_z = deque(maxlen=max_samples)
        self.tp_data = deque(maxlen=max_samples)
    
    def add_sample(self, intensity_x: float, intensity_y: float, intensity_z: float, tp: float) -> None:
        self.intensity_x.append(intensity_x)
        self.intensity_y.append(intensity_y)
        self.intensity_z.append(intensity_z)
        self.tp_data.append(tp)
    