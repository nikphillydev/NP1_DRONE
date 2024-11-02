from collections import deque


class SensorData():
    """ This class holds the samples of various sensors.
    """
    def __init__(self, max_samples: int) -> None:
        self.acc_samples = AccelerometerSamples(max_samples)
        self.gyro_samples = GyroscopeSamples(max_samples)
        self.temperature_samples = TemperatureSamples(max_samples)
        
    def add_accelerometer_sample(self, x: float, y: float, z: float, tp: float) -> None:
        self.acc_samples.add_sample(x, y, z, tp)
    
    def add_gyroscope_sample(self, x: float, y: float, z: float, tp: float) -> None:
        self.gyro_samples.add_sample(x, y, z, tp)
        
    def add_temperature_sample(self, temp: float, tp: float) -> None:
        self.temperature_samples.add_sample(temp, tp)
    
    
class AccelerometerSamples():
    """ This class holds accelerometer data samples.
    """
    def __init__(self, max_samples: int) -> None:
        self.acc_x_data = deque(maxlen=max_samples)
        self.acc_y_data = deque(maxlen=max_samples)
        self.acc_z_data = deque(maxlen=max_samples)
        self.tp_data = deque(maxlen=max_samples)
    
    def add_sample(self, x: float, y: float, z: float, tp: float) -> None:
        self.acc_x_data.append(x)
        self.acc_y_data.append(y)
        self.acc_z_data.append(z)
        self.tp_data.append(tp)
        
        
class GyroscopeSamples():
    """ This class holds gyroscope data samples.
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
        

class TemperatureSamples():
    """ This class holds temperature data samples.
    """
    def __init__(self, max_samples: int) -> None:
        self.temp_data = deque(maxlen=max_samples)
        self.tp_data = deque(maxlen=max_samples)
    
    def add_sample(self, temp: float, tp: float) -> None:
        self.temp_data.append(temp)
        self.tp_data.append(tp)
    