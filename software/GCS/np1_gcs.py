import sys
from PyQt6.QtCore import QTimer, Qt
from PyQt6.QtWidgets import (
    QApplication, QMainWindow, QGridLayout, QWidget
)

from SerialParser.serial_parser import SerialParserThread, SensorData, LoggingData, Severity
from Widgets.plot_widget import VelocityPlot
from Widgets.log_widget import LogWidget
from Widgets.aircraft_display_widget import AircraftDisplayWidget
from Widgets.altimeter_widget import AltimeterWidget


def main():
    # Configuration
    usb_port = "/dev/ttyACM0"
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
        
        # Create custom widgets
        self.vel_graph = VelocityPlot(sensor_data.state)
        self.model = AircraftDisplayWidget(sensor_data.state)
        self.altimeter = AltimeterWidget(sensor_data.state)
        grid_layout.addWidget(self.vel_graph, 0, 0) 
        grid_layout.addWidget(self.model, 0, 1)     
        grid_layout.addWidget(self.altimeter, 0, 2, Qt.AlignmentFlag.AlignCenter)
        
        # Add logging outputs
        self.logging_data = logging_data
        
        self.info_log_box = LogWidget("Information Logs")
        grid_layout.addWidget(self.info_log_box, 1, 0)
        self.critical_log_box = LogWidget("Critical Logs")
        grid_layout.addWidget(self.critical_log_box, 1, 1)
        self.error_log_box = LogWidget("Error Logs")
        grid_layout.addWidget(self.error_log_box, 1, 2)
        
        # Setup a timer to trigger the redraw of widgets.
        self.graph_timer = QTimer()
        self.graph_timer.setInterval(100)
        self.graph_timer.timeout.connect(self.update_plots)
        self.graph_timer.start()

    def update_plots(self):
        self.vel_graph.update_plot()
        self.model.update_plot()
        self.altimeter.update_altitude()
        
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

