import sys
from PyQt6.QtCore import QTimer
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QGridLayout, QWidget
)

from SerialParser.serial_parser import SerialParserThread, SensorData, LoggingData, Severity
from Widgets.plot_widget import AccelerometerPlot, GyroscopePlot, AltitudePlot
from Widgets.sensor_widget import SensorDataWidget
from Widgets.log_widget import LogWidget


def main():
    # Configuration
    usb_port = "/dev/ttyACM1"
    max_saved_samples = 100
    max_saved_logs = 30
    
    # Structures for sensor and logging data
    sensor_data = SensorData(max_samples=max_saved_samples)
    logging_data = LoggingData(max_logs=max_saved_logs)
    
    # --- Application ---
    
    serial_parser = SerialParserThread(usb_port, sensor_data, logging_data)
    serial_parser.start()

    app = QApplication(sys.argv)
    main_window = MainWindow(sensor_data, logging_data)
    main_window.show()
    
    # Connect signals
    serial_parser.added_log.connect(main_window.update_log)
    
    sys.exit(app.exec())


class MainWindow(QMainWindow):
    def __init__(self, sensor_data: SensorData, logging_data: LoggingData):
        super().__init__()

        # Set up the main window
        self.setWindowTitle("NP1 Ground Control Station")
        self.setGeometry(100, 100, 1800, 1200)

        # Create the central widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        # Create a grid layout
        grid_layout = QGridLayout()
        central_widget.setLayout(grid_layout)
        
        # Create Matplotlib graph widgets for sensors
        self.acc_graph = AccelerometerPlot(sensor_data.acc_samples)
        self.gyro_graph = GyroscopePlot(sensor_data.gyro_samples)
        self.alt_graph = AltitudePlot(sensor_data.bar_samples)
        grid_layout.addWidget(self.acc_graph, 0, 0)  # (1,1) in grid
        grid_layout.addWidget(self.gyro_graph, 0, 1)  # (1,2) in grid
        grid_layout.addWidget(self.alt_graph, 0, 2)  # (1,3) in grid
        
        # Add sensor data box
        self.sensor_data_box = SensorDataWidget(sensor_data)
        grid_layout.addWidget(self.sensor_data_box, 0, 3) # (1,4) in grid
        
        # Add logging outputs
        self.logging_data = logging_data
        
        self.info_log_box = LogWidget("Information Logs")
        grid_layout.addWidget(self.info_log_box, 1, 0) # (2,1) in grid
        self.critical_log_box = LogWidget("Critical Logs")
        grid_layout.addWidget(self.critical_log_box, 1, 1) # (2,2) in grid
        self.error_log_box = LogWidget("Error Logs")
        grid_layout.addWidget(self.error_log_box, 1, 2) # (2,3) in grid
        
        # Setup a timer to trigger the redraw of plots.
        self.graph_timer = QTimer()
        self.graph_timer.setInterval(100)
        self.graph_timer.timeout.connect(self.update_plots)
        self.graph_timer.start()
        
        # Setup a timer to trigger update of sensor data box.
        self.sensor_timer = QTimer()
        self.sensor_timer.setInterval(50)
        self.sensor_timer.timeout.connect(self.sensor_data_box.update)
        self.sensor_timer.start()

    def update_plots(self):
        self.acc_graph.update_plot()
        self.gyro_graph.update_plot()
        self.alt_graph.update_plot()
        
    def update_log(self, severity: Severity):
        
        logs = None
        log_box = None
        if (severity == Severity.INFO):
            logs = self.logging_data.info_logs
            log_box = self.info_log_box
        if (severity == Severity.CRITICAL):
            logs = self.logging_data.critical_logs
            log_box = self.critical_log_box
        if (severity == Severity.ERROR):
            logs = self.logging_data.error_logs
            log_box = self.error_log_box
            
        log = logs[-1]
        temp_str = f"{log.tp:<8}: {log.log_string}"
        log_box.append_text(temp_str)
        
if __name__ == "__main__":
    main()

