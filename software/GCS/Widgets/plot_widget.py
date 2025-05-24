import math
from matplotlib.ticker import MaxNLocator, MultipleLocator
from matplotlib.backends.backend_qtagg import FigureCanvasQTAgg as FigureCanvas, NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure

from PyQt6.QtWidgets import QWidget, QVBoxLayout

from SerialParser.sensor_data import BMI088_AccelerometerSamples, BMI088_GyroscopeSamples, BMP388_BarometerSamples, Vehicle_State

class VelocityPlot(QWidget):
    def __init__(self, vehicle_state: Vehicle_State) -> None:
        super().__init__()
        
        # Create a Matplotlib Figure and Canvas
        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)
        self.toolbar = NavigationToolbar(self.canvas)

        # Add subplot and set y axis limits
        self.ax = self.figure.add_subplot()
        self.ax.set_ylim(-10, 10)
        
        # Add data
        self.X = vehicle_state.tp_data
        self.Y1 = vehicle_state.vel_x_data
        self.Y2 = vehicle_state.vel_y_data
        self.line1, = self.ax.plot(self.X, self.Y1, label="X")
        self.line2, = self.ax.plot(self.X, self.Y2, label="Y")
        
        # Calculate statistics
        self.calculate_statistics()      

        # Add text annotation for statistics
        self.stats_text = self.ax.text(
            0.02, 0.02, 
            self.get_stats_text(),
            transform=self.ax.transAxes,
            bbox=dict(facecolor='white', alpha=0.7, edgecolor='gray'),
            fontsize=13,
            horizontalalignment='left',
            verticalalignment='bottom' 
        )

        # Format graph
        self.ax.xaxis.set_major_locator(MaxNLocator(nbins=10, integer=True))
        self.ax.tick_params(axis='x', labelrotation=15)
        self.ax.set_xlabel("Time since startup (s)")
        self.ax.set_ylabel("Velocity (cm/s)")
        self.ax.set_title("XY Velocity")
        self.ax.legend(loc='upper left')
        
        # Adjust layout manually to avoid clipping
        self.figure.subplots_adjust(left=0.2, right=0.95, top=0.95, bottom=0.15)
        
        # Initialize the layout and add the canvas
        layout = QVBoxLayout()
        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)
        self.setLayout(layout)
    
    def calculate_statistics(self):
        """Calculate statistics for each axis."""
        # Calculate averages
        self.avg_x = sum(self.Y1) / len(self.Y1) if self.Y1 else 0
        self.avg_y = sum(self.Y2) / len(self.Y2) if self.Y2 else 0
        
        # Find peak values (maximum absolute value)
        self.peak_x = max(abs(x) for x in self.Y1) if self.Y1 else 0
        self.peak_y = max(abs(y) for y in self.Y2) if self.Y2 else 0
        
        # Get newest values
        self.newest_x = self.Y1[-1] if self.Y1 else 0
        self.newest_y = self.Y2[-1] if self.Y2 else 0
    
    def get_stats_text(self):
        """Format the statistics text."""
        return (
            f"X: Avg={self.avg_x:.3f}  Peak={self.peak_x:.3f}  Current={self.newest_x:.3f} cm/s\n"
            f"Y: Avg={self.avg_y:.3f}  Peak={self.peak_y:.3f}  Current={self.newest_y:.3f} cm/s"
        )
    
    def update_plot(self) -> None:
        # Set new data
        self.line1.set_data(self.X, self.Y1)
        self.line2.set_data(self.X, self.Y2)

        # Recalculate statistics
        self.calculate_statistics()
        
        # Update the statistics text
        self.stats_text.set_text(self.get_stats_text())
        
        # Adjust x-axis limits to show all data
        self.ax.relim()
        self.ax.autoscale_view()
        
        # Trigger the canvas to update and redraw.
        self.canvas.draw_idle()

    
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
        
        # Calculate statistics
        self.calculate_statistics()      

        # Add text annotation for statistics
        self.stats_text = self.ax.text(
            0.02, 0.02, 
            self.get_stats_text(),
            transform=self.ax.transAxes,
            bbox=dict(facecolor='white', alpha=0.7, edgecolor='gray'),
            fontsize=10,
            verticalalignment='bottom'
        )

        # Format graph
        self.ax.xaxis.set_major_locator(MaxNLocator(nbins=10, integer=True))
        self.ax.tick_params(axis='x', labelrotation=15)
        self.ax.set_xlabel("Time since startup (s)")
        self.ax.set_ylabel("Acceleration (m/s^2)")
        self.ax.set_title("Accelerometer Data")
        self.ax.legend(loc='upper left')
        
        # Adjust layout manually to avoid clipping
        self.figure.subplots_adjust(left=0.2, right=0.95, top=0.95, bottom=0.15)
        
        # Initialize the layout and add the canvas
        layout = QVBoxLayout()
        layout.addWidget(self.toolbar)
        layout.addWidget(self.canvas)
        self.setLayout(layout)
    
    def calculate_statistics(self):
        """Calculate statistics for each axis."""
        # Calculate averages
        self.avg_x = sum(self.Y1) / len(self.Y1) if self.Y1 else 0
        self.avg_y = sum(self.Y2) / len(self.Y2) if self.Y2 else 0
        self.avg_z = sum(self.Y3) / len(self.Y3) if self.Y3 else 0
        
        # Find peak values (maximum absolute value)
        self.peak_x = max(abs(x) for x in self.Y1) if self.Y1 else 0
        self.peak_y = max(abs(y) for y in self.Y2) if self.Y2 else 0
        self.peak_z = max(abs(z) for z in self.Y3) if self.Y3 else 0
        
        # Get newest values
        self.newest_x = self.Y1[-1] if self.Y1 else 0
        self.newest_y = self.Y2[-1] if self.Y2 else 0
        self.newest_z = self.Y3[-1] if self.Y3 else 0
    
    def get_stats_text(self):
        """Format the statistics text."""
        return (
            f"X: Avg={self.avg_x:.3f}  Peak={self.peak_x:.3f}  Current={self.newest_x:.3f} m/s²\n"
            f"Y: Avg={self.avg_y:.3f}  Peak={self.peak_y:.3f}  Current={self.newest_y:.3f} m/s²\n"
            f"Z: Avg={self.avg_z:.3f}  Peak={self.peak_z:.3f}  Current={self.newest_z:.3f} m/s²"
        )
    
    def update_plot(self) -> None:
        # Set new data
        self.line1.set_data(self.X, self.Y1)
        self.line2.set_data(self.X, self.Y2)
        self.line3.set_data(self.X, self.Y3)

        # Recalculate statistics
        self.calculate_statistics()
        
        # Update the statistics text
        self.stats_text.set_text(self.get_stats_text())
        
        # Adjust x-axis limits to show all data
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
        

class BarometerPlot(QWidget):
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
        self.Y1 = altitude_samples.altitude_data
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