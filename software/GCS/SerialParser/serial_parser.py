import threading
import serial

from SensorData.sensor_data import SensorData
from LoggingData.logging_data import LoggingData


class SerialParserThread(threading.Thread):
    """ This thread reads and parses serial data into sensor or logging information.
    """
    def __init__(self, port: str, sensor_data: SensorData, logging_data: LoggingData) -> None:
        super().__init__()
        self.ser = serial.Serial(port)
        self.sensor_data = sensor_data
        self.logging_data = logging_data
        
    def run(self) -> None:
        while(True):
            line = self.ser.readline()
            string = line.decode("utf-8")
            string_split = string.split()
            
            if "SENSOR" in string_split:
                string_split.remove("SENSOR")
                self.parse_sensor_data(string_split)
            
            if "LOG" in string_split:
                string_split.remove("LOG")
                self.parse_logging_data(string_split)
    
    def parse_sensor_data(self, string_split: list[str]) -> None:
        tp = float(string_split[0]) 
        if 'ACC' in string_split:
            string_split.remove('ACC')
            x = float(string_split[1])
            y = float(string_split[2])   
            z = float(string_split[3])   
            self.sensor_data.add_accelerometer_sample(x, y, z, tp)
            
        if "GYRO" in string_split:
            string_split.remove("GYRO")
            x = float(string_split[1])
            y = float(string_split[2])   
            z = float(string_split[3])   
            self.sensor_data.add_gyroscope_sample(x, y, z, tp)
            
        if "TEMP" in string_split:
            string_split.remove("TEMP")   
            temp = float(string_split[1])
            self.sensor_data.add_temperature_sample(temp, tp)
    
    def parse_logging_data(self, string_split: list[str]) -> None:
        tp = string_split[0]
        del string_split[0]
        if "INFO" in string_split:
            string_split.remove('INFO')
            string = " ".join(string_split)
            self.logging_data.add_info_log(string, tp)
            
        if "WARN" in string_split:
            string_split.remove('WARN')
            string = " ".join(string_split)
            self.logging_data.add_warn_log(string, tp)
            
        if "ERR" in string_split:
            string_split.remove('ERR')
            string = " ".join(string_split)
            self.logging_data.add_err_log(string, tp)