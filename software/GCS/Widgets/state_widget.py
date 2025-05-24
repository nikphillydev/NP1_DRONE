from PyQt6.QtWidgets import QWidget, QLabel, QLineEdit, QHBoxLayout, QVBoxLayout, QGroupBox, QSpacerItem, QSizePolicy

from SerialParser.sensor_data import Vehicle_State

class StateDataWidget(QWidget):
    def __init__(self, state: Vehicle_State):
        super().__init__()

        self.state = state
        
        # Main layout for the widget
        main_layout = QVBoxLayout()

        # Create groups for each sensor
        self.orientation_group = self.create_sensor_group(
            "Orientation", 
            ["Roll:", "Pitch:", "Yaw:"],
            ["roll", "pitch", "yaw"]
        )
        main_layout.addWidget(self.orientation_group)

        self.velocity_group = self.create_sensor_group(
            "Velocity (Body Frame) (m/s)", 
            ["Velocity X:", "Velocity Y:"],
            ["vel_x", "vel_y"]
        )
        main_layout.addWidget(self.velocity_group)

        self.altitude_group = self.create_sensor_group(
            "Altitude", 
            ["Altitude (m):"],
            ["alt"],
            max_columns=1
        )
        main_layout.addWidget(self.altitude_group)

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
        # Update with the most recent state data
            
        # Vehicle State
        self.roll.setText(str(self.state.roll))
        self.pitch.setText(str(self.state.pitch))
        self.yaw.setText(str(self.state.yaw))
        if len(self.state.vel_x_data) > 0:
            self.vel_x.setText(str(self.state.vel_x_data[-1]))
            self.vel_y.setText(str(self.state.vel_y_data[-1]))
        self.alt.setText(str(self.state.altitude))