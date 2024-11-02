import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
import math

from SensorData.sensor_data import AccelerometerSamples, GyroscopeSamples


class AccelerometerPlot():
    def __init__(self, acc_samples: AccelerometerSamples) -> None:
        self.figure = plt.figure()
        self.ax = self.figure.add_subplot()
        self.ax.set_ylim(-20, 20)
        
        self.X = acc_samples.tp_data
        self.Y1 = acc_samples.acc_x_data
        self.Y2 = acc_samples.acc_y_data
        self.Y3 = acc_samples.acc_z_data
        self.line1, = self.ax.plot(self.X, self.Y1, label="X")
        self.line2, = self.ax.plot(self.X, self.Y2, label="Y")
        self.line3, = self.ax.plot(self.X, self.Y3, label="Z")
        
        self.ax.xaxis.set_major_locator(MaxNLocator(nbins=10, integer=True))
        self.ax.tick_params(axis='x', labelrotation=15)
        self.ax.set_xlabel("Time since startup (s)")
        self.ax.set_ylabel("Acceleration (m/s^2)")
        self.ax.set_title("Accelerometer Data")
        self.ax.legend(loc='lower left')
                
    def update_plot(self) -> None:                  
        self.line1.set_data(self.X, self.Y1)
        self.line2.set_data(self.X, self.Y2)
        self.line3.set_data(self.X, self.Y3)
        self.ax.relim()
        self.ax.autoscale_view()
                    
        
class GyroscopePlot():
    def __init__(self, gyro_samples: GyroscopeSamples) -> None:
        self.figure = plt.figure()
        self.ax = self.figure.add_subplot()
        self.ax.set_ylim(-2*math.pi, 2*math.pi)
        
        self.X = gyro_samples.tp_data
        self.Y1 = gyro_samples.rate_x_data
        self.Y2 = gyro_samples.rate_y_data
        self.Y3 = gyro_samples.rate_z_data
        self.line1, = self.ax.plot(self.X, self.Y1, label="X")
        self.line2, = self.ax.plot(self.X, self.Y2, label="Y")
        self.line3, = self.ax.plot(self.X, self.Y3, label="Z")
        
        self.ax.xaxis.set_major_locator(MaxNLocator(nbins=10, integer=True))
        self.ax.tick_params(axis='x', labelrotation=15)
        self.ax.set_xlabel("Time since startup (s)")
        self.ax.set_ylabel("Angular Velocity (rads/s)")
        self.ax.set_title("Gyroscope Data")
        self.ax.legend(loc='lower left')
                
    def update_plot(self) -> None:          
        self.line1.set_data(self.X, self.Y1)
        self.line2.set_data(self.X, self.Y2)
        self.line3.set_data(self.X, self.Y3)
        self.ax.relim()
        self.ax.autoscale_view()