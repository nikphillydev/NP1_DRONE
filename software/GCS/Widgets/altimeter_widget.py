from PyQt6.QtWidgets import QWidget
from PyQt6.QtGui import QPainter, QFont, QColor, QPen
from PyQt6.QtCore import Qt, QRectF, QPointF

from SerialParser.sensor_data import Vehicle_State


class AltimeterWidget(QWidget):
    """
    A PyQt6 widget that displays a vertical scrolling altimeter based on
    the altitude value from a Vehicle_State instance.
    """
    def __init__(self, vehicle_state: Vehicle_State, parent=None):
        super().__init__(parent)
        self.vehicle_state = vehicle_state
        self.setMinimumSize(350, 600)
        self.setMaximumWidth(500)

        self.setWindowTitle("Vehicle Altimeter")

        # Configuration for the altimeter display
        self.tick_spacing_cm = 10       # Major tick every X cm
        self.minor_tick_count = 10      # X minor ticks between major ticks
        self.visible_ticks = 5          # Approximately X major ticks visible on screen
        
        # Calculate pixels per centimeter based on desired visible ticks and widget height
        # Initial estimate, will be refined in paintEvent based on actual height
        self.pixels_per_cm = self.height() / (self.visible_ticks * self.tick_spacing_cm) 
        
        # Font for numbers
        self.font = QFont("Inter", 15)

        # Current altitude in centimeters for drawing
        self._current_altitude_cm = self.vehicle_state.altitude * 100

    def update_altitude(self) -> None:
        """
        Updates the altimeter display based on the current altitude in the
        Vehicle_State instance. This method should be called externally
        whenever the vehicle_state.altitude changes.
        """
        # Convert meters to centimeters for internal drawing logic
        new_altitude_cm = self.vehicle_state.altitude * 100
        
        # Only redraw if altitude has actually changed significantly
        if abs(self._current_altitude_cm - new_altitude_cm) > 0.1: # Small threshold for redraw
            self._current_altitude_cm = new_altitude_cm
            self.update() # Triggers a repaint event

    def paintEvent(self, event) -> None:
        """
        Handles the painting of the altimeter widget.
        Draws the scrolling ticks, numbers, and the current altitude indicator.
        """
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)

        width = self.width()
        height = self.height()

        # Calculate pixels per cm dynamically based on current widget height
        # This ensures approximately 10 major ticks are always visible
        self.pixels_per_cm = height / (self.visible_ticks * self.tick_spacing_cm)
        
        # Background color
        painter.fillRect(self.rect(), QColor(30, 30, 30)) # Dark background

        # Set font for text (default for ticks/numbers)
        painter.setFont(self.font)
        painter.setPen(QColor(200, 200, 200)) # Light gray text color

        # --- Add Title Text "Altimeter" ---
        title_font = QFont("Inter", 20, QFont.Weight.Bold) # Larger, bold font for title
        painter.setFont(title_font)
        painter.setPen(QColor(255, 255, 255)) # White color for title
        
        title_text = "Altimeter"
        # Position the title in the top-left, with some padding
        title_x = 10
        title_y = painter.fontMetrics().height() + 5 # Adjust based on font height
        painter.drawText(title_x, title_y, title_text)

        # Restore default font/pen for the rest of the altimeter drawing
        painter.setFont(self.font)
        painter.setPen(QColor(200, 200, 200))

        # --- Draw Ticks and Numbers ---
        
        # Determine the range of altitudes to draw based on the current altitude
        # and the visible area.
        # The center of the widget corresponds to the current altitude.
        center_y = height / 2

        # Find the lowest and highest altitude values (in cm) that might be visible
        # We need to extend beyond the visible height to ensure smooth scrolling
        # and to draw ticks that are partially off-screen.
        buffer_cm = (self.visible_ticks * self.tick_spacing_cm) / 2 + self.tick_spacing_cm * 2
        min_alt_cm = self._current_altitude_cm - buffer_cm
        max_alt_cm = self._current_altitude_cm + buffer_cm

        # Round down to the nearest major tick for the start point
        start_tick_cm = int(min_alt_cm / self.tick_spacing_cm) * self.tick_spacing_cm
        
        # Draw ticks and numbers
        current_cm = start_tick_cm
        while current_cm <= max_alt_cm:
            # Calculate the y-position for the current tick
            # The y-position is relative to the center_y, adjusted by the difference
            # from the current altitude and scaled by pixels_per_cm.
            y_pos = center_y - (current_cm - self._current_altitude_cm) * self.pixels_per_cm

            # Only draw if the tick is within or slightly outside the widget bounds
            if -50 < y_pos < height + 50: # Add a buffer for partial ticks
                # Major ticks
                if current_cm % self.tick_spacing_cm == 0:
                    painter.setPen(QPen(QColor(255, 255, 255), 2)) # White, thicker line
                    painter.drawLine(width - 80, int(y_pos), width - 20, int(y_pos)) # Long tick

                    # Draw number
                    text = f"{current_cm} cm"
                    text_rect = QRectF(0, y_pos - painter.fontMetrics().height() / 2, width - 85, painter.fontMetrics().height())
                    painter.drawText(text_rect, Qt.AlignmentFlag.AlignRight | Qt.AlignmentFlag.AlignVCenter, text)
                
                # Minor ticks (between major ticks)
                else:
                    painter.setPen(QPen(QColor(150, 150, 150), 1)) # Gray, thinner line
                    painter.drawLine(width - 60, int(y_pos), width - 20, int(y_pos)) # Shorter tick
            
            # Increment to the next minor tick
            current_cm += self.tick_spacing_cm / self.minor_tick_count

        # --- Draw Central Indicator ---
        indicator_height = 20
        # indicator_width = width - 10 # Almost full width
        
        # Main indicator line
        painter.setPen(QPen(QColor(255, 0, 0), 3)) # Red, thick line
        painter.drawLine(10, int(center_y), width - 10, int(center_y))

        # Triangle indicator on the right, pointing right
        painter.setBrush(QColor(255, 0, 0)) # Red fill
        points = [
            QPointF(width - 60, center_y), # Tip of the triangle at the right edge
            QPointF(width - 80, center_y - indicator_height / 2),
            QPointF(width - 80, center_y + indicator_height / 2)
        ]
        painter.drawPolygon(points)

        # Display current altitude value prominently
        painter.setFont(QFont("Inter", 20, QFont.Weight.Bold))
        painter.setPen(QColor(255, 255, 0)) # Yellow for current value
        current_alt_text = f"{self._current_altitude_cm:.1f} cm"
        # Adjusted text_rect to move the text to the left and center it within the left half
        text_rect = QRectF(0, center_y - 50, width / 2, 40)
        painter.drawText(text_rect, Qt.AlignmentFlag.AlignCenter | Qt.AlignmentFlag.AlignVCenter, current_alt_text)

        painter.end()

