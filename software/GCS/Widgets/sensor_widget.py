from PyQt6.QtWidgets import QWidget, QLabel, QLineEdit, QHBoxLayout, QVBoxLayout, QGroupBox, QSpacerItem, QSizePolicy

from SerialParser.serial_parser import SensorData


class SensorDataWidget(QWidget):
    def __init__(self, sensor_data: SensorData):
        super().__init__()

        self.sensor_data = sensor_data
        
        # Main layout for the widget
        main_layout = QVBoxLayout()

        # Create groups for each sensor
        self.accelerometer_group = self.create_sensor_group(
            "Accelerometer (m/s^2)", 
            ["X:", "Y:", "Z:", "Temperature (degC):"],
            ["acc_x", "acc_y", "acc_z", "acc_temp"]
        )
        main_layout.addWidget(self.accelerometer_group)

        self.gyroscope_group = self.create_sensor_group(
            "Gyroscope (rads/s)", 
            ["RateX:", "RateY:", "RateZ:"],
            ["gyro_x", "gyro_y", "gyro_z"]
        )
        main_layout.addWidget(self.gyroscope_group)

        self.barometer_group = self.create_sensor_group(
            "Barometer", 
            ["Altitude (m):", "Pressure (Pa):", "Temperature (degC):"],
            ["bar_alt", "bar_press", "bar_temp"],
            max_columns=1
        )
        main_layout.addWidget(self.barometer_group)

        # Set the main layout
        self.setLayout(main_layout)
        self.setMaximumWidth(600)  # Limit the width of the entire widget

    def create_sensor_group(self, title, labels, field_names, max_columns=3):
        group_box = QGroupBox(title)
        group_box.setStyleSheet("QGroupBox { font-weight: bold; font-size: 12pt; color: darkblue; }")
        
        layout = QVBoxLayout()
        
        # Create a row layout for the label-field pairs
        row_layout = QHBoxLayout()
        
        for idx, (label_text, field_name) in enumerate(zip(labels, field_names)):
            label = QLabel(label_text)
            text_box = QLineEdit()
            text_box.setReadOnly(True)
            text_box.setMaximumWidth(80)
            setattr(self, field_name, text_box)

            row_layout.addWidget(label)
            row_layout.addWidget(text_box)

            if (idx + 1) % max_columns == 0:
                # If we've added the maximum number of columns, start a new row
                layout.addLayout(row_layout)
                row_layout = QHBoxLayout()
        
        if row_layout.count() > 0:
            # Add a spacer to fill the remaining empty space in the row
            remaining_spaces = max_columns - row_layout.count()
            for _ in range(remaining_spaces):
                spacer = QSpacerItem(20, 20, QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Minimum)
                row_layout.addItem(spacer)
            layout.addLayout(row_layout)

        group_box.setLayout(layout)
        return group_box

    
    def update(self):
        # Update with the most recent sensor data
        
        # Accelerometer
        self.acc_x.setText(str(self.sensor_data.acc_samples.acc_x_data[-1]))
        self.acc_y.setText(str(self.sensor_data.acc_samples.acc_y_data[-1]))
        self.acc_z.setText(str(self.sensor_data.acc_samples.acc_z_data[-1]))
        self.acc_temp.setText(str(self.sensor_data.acc_samples.acc_temp_data[-1]))
        
        # Gyroscope
        self.gyro_x.setText(str(self.sensor_data.gyro_samples.rate_x_data[-1]))
        self.gyro_y.setText(str(self.sensor_data.gyro_samples.rate_y_data[-1]))
        self.gyro_z.setText(str(self.sensor_data.gyro_samples.rate_z_data[-1]))
        
        # Barometer
        self.bar_alt.setText(str(self.sensor_data.bar_samples.bar_alt_data[-1]))
        self.bar_press.setText(str(self.sensor_data.bar_samples.bar_press_data[-1]))
        self.bar_temp.setText(str(self.sensor_data.bar_samples.bar_temp_data[-1]))
