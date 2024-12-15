import math
from matplotlib.ticker import MaxNLocator, MultipleLocator
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas, NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure

from PyQt6.QtWidgets import QWidget, QVBoxLayout

from SerialParser.sensor_data import BMI088_AccelerometerSamples, BMI088_GyroscopeSamples, BMP388_BarometerSamples


class AccelerometerPlot(QWidget):
    def __init__(self, acc_samples: BMI088_AccelerometerSamples) -> None:
        super().__init__()
        
        # Create a Matplotlib Figure and Canvas
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas)

        # Add subplot and set y axis limits
        self.ax = self.figure.add_subplot()
        self.ax.set_ylim(-20, 20)
        
        # Add data
        self.X = acc_samples.tp_data
        self.Y1 = acc_samples.acc_x_data
        self.Y2 = acc_samples.acc_y_data
        self.Y3 = acc_samples.acc_z_data
        self.line1, = self.ax.plot(self.X, self.Y1, label="X")
        self.line2, = self.ax.plot(self.X, self.Y2, label="Y")
        self.line3, = self.ax.plot(self.X, self.Y3, label="Z")
        
        # Format graph
        self.ax.xaxis.set_major_locator(MaxNLocator(nbins=10, integer=True))
        self.ax.tick_params(axis='x', labelrotation=15)
        self.ax.set_xlabel("Time since startup (s)")
        self.ax.set_ylabel("Acceleration (m/s^2)")
        self.ax.set_title("Accelerometer Data")
        self.ax.legend(loc='lower left')
        
        # Adjust layout manually to avoid clipping
        self.figure.subplots_adjust(left=0.2, right=0.95, top=0.95, bottom=0.15)
        
        # Initialize the layout and add the canvas
        layout = QVBoxLayout()
        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)
        self.setLayout(layout)
                
    def update_plot(self) -> None:
        # Set new data
        self.line1.set_data(self.X, self.Y1)
        self.line2.set_data(self.X, self.Y2)
        self.line3.set_data(self.X, self.Y3)
        self.ax.relim()
        self.ax.autoscale_view()
        
        # Trigger the canvas to update and redraw.
        self.canvas.draw_idle()
                    
        
class GyroscopePlot(QWidget):
    def __init__(self, gyro_samples: BMI088_GyroscopeSamples) -> None:
        super().__init__()
        
        # Create a Matplotlib Figure and Canvas
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas)
        
        # Add subplot and set y axis limits
        self.ax = self.figure.add_subplot()
        self.ax.set_ylim(-2*math.pi, 2*math.pi)
        
        # Add data
        self.X = gyro_samples.tp_data
        self.Y1 = gyro_samples.rate_x_data
        self.Y2 = gyro_samples.rate_y_data
        self.Y3 = gyro_samples.rate_z_data
        self.line1, = self.ax.plot(self.X, self.Y1, label="X")
        self.line2, = self.ax.plot(self.X, self.Y2, label="Y")
        self.line3, = self.ax.plot(self.X, self.Y3, label="Z")
        
        # Format graph
        self.ax.xaxis.set_major_locator(MaxNLocator(nbins=10, integer=True))
        self.ax.tick_params(axis='x', labelrotation=15)
        self.ax.set_xlabel("Time since startup (s)")
        self.ax.set_ylabel("Angular Velocity (rads/s)")
        self.ax.set_title("Gyroscope Data")
        self.ax.legend(loc='lower left')
        
        # Adjust layout manually to avoid clipping
        self.figure.subplots_adjust(left=0.2, right=0.95, top=0.95, bottom=0.15)
        
        # Initialize the layout and add the canvas
        layout = QVBoxLayout()
        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)
        self.setLayout(layout)
                
    def update_plot(self) -> None:          
        # Set new data
        self.line1.set_data(self.X, self.Y1)
        self.line2.set_data(self.X, self.Y2)
        self.line3.set_data(self.X, self.Y3)
        self.ax.relim()
        self.ax.autoscale_view()
        
        # Trigger the canvas to update and redraw.
        self.canvas.draw_idle()
        

class AltitudePlot(QWidget):
    def __init__(self, altitude_samples: BMP388_BarometerSamples) -> None:
        super().__init__()
        
        # Create a Matplotlib Figure and Canvas
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas)
        
        # Add subplot and set y axis limits
        self.ax = self.figure.add_subplot()
        self.ax.set_ylim(-3, 3)
        
        # Add data
        self.X = altitude_samples.tp_data
        self.Y1 = altitude_samples.bar_alt_data
        self.line1, = self.ax.plot(self.X, self.Y1, label="Height")
        
        # Format graph
        self.ax.yaxis.set_minor_locator(MultipleLocator(0.25))  # Minor ticks at half-unit intervals
        self.ax.xaxis.set_major_locator(MaxNLocator(nbins=10, integer=True))        
        self.ax.tick_params(axis='x', labelrotation=15)
        self.ax.set_xlabel("Time since startup (s)")
        self.ax.set_ylabel("Height above startup (m)")
        self.ax.set_title("Altitude Data")
        self.ax.legend(loc='lower left')
        
        # Add soft horizontal grid lines for both major and minor ticks
        self.ax.grid(which='major', axis='y', linestyle='--', linewidth=0.8, alpha=0.8, color='gray')
        self.ax.grid(which='minor', axis='y', linestyle=':', linewidth=0.8, alpha=0.8, color='gray')
        
        # Adjust layout manually to avoid clipping
        self.figure.subplots_adjust(left=0.2, right=0.95, top=0.95, bottom=0.15)
        
        # Initialize the layout and add the canvas
        layout = QVBoxLayout()
        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)
        self.setLayout(layout)
                
    def update_plot(self) -> None:          
         # Set new data
        self.line1.set_data(self.X, self.Y1)
        self.ax.relim()
        self.ax.autoscale_view()
                
        # Trigger the canvas to update and redraw.
        self.canvas.draw_idle()