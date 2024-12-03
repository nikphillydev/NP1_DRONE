import serial
import time
from PyQt6.QtCore import QThread, pyqtSignal

from SerialParser.sensor_data import SensorData
from SerialParser.logging_data import LoggingData, Severity


class SerialParserThread(QThread):
    """ This thread reads and parses serial data into sensor or logging information.
    """
    
    added_log = pyqtSignal(Severity)
    
    def __init__(self, port: str, sensor_data: SensorData, logging_data: LoggingData) -> None:
        super().__init__()
        
        self.port = port
        self.ser = serial.Serial(port)
        self.sensor_data = sensor_data
        self.logging_data = logging_data
        self.running = True
        
    def run(self) -> None:
        while(self.running):
            try:
                line = self.ser.readline()
                string = line.decode("utf-8")
                string_split = string.split()
                
                if string_split[0] == "SENSOR":
                    del string_split[0]
                    self.parse_sensor_data(string_split)
                
                if string_split[0] == "LOG":
                    del string_split[0]
                    self.parse_logging_data(string_split)
                    
            except serial.SerialException:
                print("Serial port with FCC closed, attempting to reopen port...")
                self.ser.close()
                time.sleep(0.5)
                self.ser.open()
                
    def stop(self):
        self.running = False
        self.wait()  # Wait for the thread to finish
    
    def parse_sensor_data(self, string_split: list[str]) -> None:
        """ Parse a string list into relevant sensor data. """
        
        # Parse timepoint
        tp = float(string_split[0])
        del string_split[0]
        
        # Parse sensor data
        
        if string_split[0] == "ACC":
            x = float(string_split[1])
            y = float(string_split[2])   
            z = float(string_split[3])  
            temp = float(string_split[4])  
            self.sensor_data.add_accelerometer_sample(x, y, z, temp, tp)
            
        if string_split[0] == "GYRO":
            x = float(string_split[1])
            y = float(string_split[2])   
            z = float(string_split[3])   
            self.sensor_data.add_gyroscope_sample(x, y, z, tp)
        
        if string_split[0] == "BAR":
            pressure = float(string_split[1])
            temp = float(string_split[2])   
            altitude = float(string_split[3])  
            self.sensor_data.add_barometer_sample(pressure, temp, altitude, tp)
    
    def parse_logging_data(self, string_split: list[str]) -> None:
        """ Parse a string list into relevant logging data. """
        
        # Parse timepoint
        tp = float(string_split[0])
        del string_split[0]
        
        # Parse logging information
        
        severity = None
        if string_split[0] == "INFO":
            del string_split[0]
            severity = Severity.INFO
        if string_split[0] == "WARN":
            del string_split[0]
            severity = Severity.WARNING
        if string_split[0] == "ERR":
            del string_split[0]
            severity = Severity.ERR
        
        string = " ".join(string_split)
        self.logging_data.add_log(string, tp, severity)
        self.added_log.emit(severity)