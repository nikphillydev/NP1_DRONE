from collections import deque

class SensorData():
    """ This class holds data samples of various sensors.
    """
    def __init__(self, max_samples: int) -> None:
        self.acc_samples = BMI088_AccelerometerSamples(max_samples)
        self.gyro_samples = BMI088_GyroscopeSamples(max_samples)
        self.bar_samples = BMP388_BarometerSamples(max_samples)
        
    def add_accelerometer_sample(self, x: float, y: float, z: float, temp: float, tp: float) -> None:
        self.acc_samples.add_sample(x, y, z, temp, tp)
    
    def add_gyroscope_sample(self, x: float, y: float, z: float, tp: float) -> None:
        self.gyro_samples.add_sample(x, y, z, tp)
    
    def add_barometer_sample(self, pressure: float, temperature: float, altitude: float, tp: float) -> None:
        self.bar_samples.add_sample(pressure, temperature, altitude, tp)
    
    
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
        self.bar_press_data = deque(maxlen=max_samples)
        self.bar_temp_data = deque(maxlen=max_samples)
        self.bar_alt_data = deque(maxlen=max_samples)
        self.tp_data = deque(maxlen=max_samples)
    
    def add_sample(self, pressure: float, temperature: float, altitude: float, tp: float) -> None:
        self.bar_press_data.append(pressure)
        self.bar_temp_data.append(temperature)
        self.bar_alt_data.append(altitude)
        self.tp_data.append(tp)
    