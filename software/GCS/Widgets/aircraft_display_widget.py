import math
from PyQt6.QtCore import Qt
from PyQt6.QtGui import QMouseEvent, QPainter, QFont, QColor
from PyQt6.QtOpenGLWidgets import QOpenGLWidget
from OpenGL.GL import *
from OpenGL.GLU import *

from SerialParser.sensor_data import Vehicle_State


class AircraftDisplayWidget(QOpenGLWidget):
    """
    PyQt6 OpenGL widget for displaying a 3D aircraft model oriented by quaternion.
    Also displays Roll, Pitch, and Yaw values as an overlay.
    """

    def __init__(self, vehicle_state: Vehicle_State):
        super().__init__()
        self.vehicle_state = vehicle_state
        self.setMinimumSize(400, 500)
        
        # Mouse interaction variables
        self.mouse_x = 0
        self.mouse_y = 0
        self.mouse_pressed = False
        self.camera_rotation_x = 45
        self.camera_rotation_y = -45
        
        # Enable mouse tracking
        self.setMouseTracking(True)

    def initializeGL(self):
        """Initialize OpenGL settings."""
        glEnable(GL_DEPTH_TEST)
        glEnable(GL_LIGHTING)
        glEnable(GL_LIGHT0)
        glEnable(GL_COLOR_MATERIAL)

        # Set up lighting
        glLightfv(GL_LIGHT0, GL_POSITION, [1.0, 1.0, 1.0, 0.0])
        glLightfv(GL_LIGHT0, GL_DIFFUSE, [1.0, 1.0, 1.0, 1.0])
        glLightfv(GL_LIGHT0, GL_AMBIENT, [0.3, 0.3, 0.3, 1.0])

        # Set background color (lighter blue)
        glClearColor(0.6, 0.8, 1.0, 1.0)

    def resizeGL(self, width, height):
        """Handle window resize."""
        if height == 0:
            height = 1

        glViewport(0, 0, width, height)
        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(45.0, width / height, 0.1, 100.0)
        glMatrixMode(GL_MODELVIEW)

    def paintGL(self):
        """Render the 3D scene and overlay 2D text."""
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
        glLoadIdentity()

        # Ensure blending is disabled for opaque 3D rendering
        glDisable(GL_BLEND) 
        glEnable(GL_DEPTH_TEST) # Ensure depth testing is enabled

        # Position camera with user-controlled rotation
        glTranslatef(0.0, 0.0, -10.0)
        glRotatef(self.camera_rotation_x, 1.0, 0.0, 0.0)
        glRotatef(self.camera_rotation_y, 0.0, 1.0, 0.0)

        # Draw fixed NED frame axes first (before aircraft transformation)
        self._draw_fixed_axes()

        # Apply quaternion rotation with axis mapping for NED frame
        # NED: yaw->roll, pitch->yaw, roll->pitch
        self._apply_quaternion_rotation()

        # Draw the aircraft
        self._draw_aircraft()

        # Draw aircraft body axes
        self._draw_body_axes()

        # --- Overlay 2D Text using QPainter ---
        # QPainter needs to operate in 2D mode, so we save the OpenGL state
        # and then restore it after QPainter is done.
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)

        # Set font and color for the text
        painter.setFont(QFont("Inter", 15, QFont.Weight.Bold))
        painter.setPen(QColor(0, 0, 0)) # Changed text color to black

        # Get current RPY values from vehicle_state
        roll_val = self.vehicle_state.roll
        pitch_val = self.vehicle_state.pitch
        yaw_val = self.vehicle_state.yaw

        # Format the text
        roll_text = f"Roll: {roll_val:.1f}°"
        pitch_text = f"Pitch: {pitch_val:.1f}°"
        yaw_text = f"Yaw: {yaw_val:.1f}°"

        # Define text positions (top-left corner)
        text_x = 10
        text_y_start = 20
        line_height = painter.fontMetrics().height() + 5 # Add some padding

        # Draw the text
        painter.drawText(text_x, text_y_start, roll_text)
        painter.drawText(text_x, text_y_start + line_height, pitch_text)
        painter.drawText(text_x, text_y_start + 2 * line_height, yaw_text)

        painter.end() # End QPainter operations

    def _apply_quaternion_rotation(self):
        """Convert quaternion to rotation matrix and apply it with NED axis mapping."""
        qx_display = self.vehicle_state.qy
        qy_display = self.vehicle_state.qz
        qz_display = -self.vehicle_state.qx
        qw = self.vehicle_state.qw

        # Normalize quaternion
        norm = math.sqrt(qx_display*qx_display + qy_display*qy_display + qz_display*qz_display + qw*qw)
        if norm > 0:
            qx_display, qy_display, qz_display, qw = qx_display/norm, qy_display/norm, qz_display/norm, qw/norm

        # Convert quaternion to rotation matrix
        rotation_matrix = [
            1 - 2*(qy_display*qy_display + qz_display*qz_display), 2*(qx_display*qy_display - qz_display*qw),     2*(qx_display*qz_display + qy_display*qw),     0,
            2*(qx_display*qy_display + qz_display*qw),     1 - 2*(qx_display*qx_display + qz_display*qz_display), 2*(qy_display*qz_display - qx_display*qw),     0,
            2*(qx_display*qz_display - qy_display*qw),     2*(qy_display*qz_display + qx_display*qw),     1 - 2*(qx_display*qx_display + qy_display*qy_display), 0, # Corrected typo: qz_designation to qz_display
            0,                     0,                     0,                     1
        ]

        glMultMatrixf(rotation_matrix)

    def _draw_aircraft(self):
        """Draw a simplified aircraft model."""
        # Aircraft body (fuselage)
        glColor3f(0.7, 0.7, 0.8)
        glPushMatrix()
        glScalef(0.3, 0.3, 2.0)
        self._draw_cylinder()
        glPopMatrix()

        # Wings
        glColor3f(0.6, 0.6, 0.7)
        glPushMatrix()
        glTranslatef(0, 0, -0.2)
        glScalef(3.0, 0.1, 0.5)
        self._draw_cube()
        glPopMatrix()

        # Tail wing (horizontal stabilizer)
        glColor3f(0.6, 0.6, 0.7)
        glPushMatrix()
        glTranslatef(0, 0, -1.5)
        glScalef(1.0, 0.05, 0.3)
        self._draw_cube()
        glPopMatrix()

        # Vertical tail (rudder)
        glPushMatrix()
        glTranslatef(0, 0.3, -1.5)
        glScalef(0.05, 0.6, 0.3)
        self._draw_cube()
        glPopMatrix()

        # Nose cone
        glColor3f(0.8, 0.3, 0.3)
        glPushMatrix()
        glTranslatef(0, 0, 1.2)
        glScalef(0.2, 0.2, 0.4)
        self._draw_cone()
        glPopMatrix()

    def _draw_cube(self):
        """Draw a unit cube."""
        glBegin(GL_QUADS)

        # Front face
        glNormal3f(0, 0, 1)
        glVertex3f(-0.5, -0.5,  0.5)
        glVertex3f( 0.5, -0.5,  0.5)
        glVertex3f( 0.5,  0.5,  0.5)
        glVertex3f(-0.5,  0.5,  0.5)

        # Back face
        glNormal3f(0, 0, -1)
        glVertex3f(-0.5, -0.5, -0.5)
        glVertex3f(-0.5,  0.5, -0.5)
        glVertex3f( 0.5,  0.5, -0.5)
        glVertex3f( 0.5, -0.5, -0.5)

        # Top face
        glNormal3f(0, 1, 0)
        glVertex3f(-0.5,  0.5, -0.5)
        glVertex3f(-0.5,  0.5,  0.5)
        glVertex3f(0.5,  0.5,  0.5)
        glVertex3f(0.5,  0.5, -0.5)

        # Bottom face
        glNormal3f(0, -1, 0)
        glVertex3f(-0.5, -0.5, -0.5)
        glVertex3f( 0.5, -0.5, -0.5)
        glVertex3f( 0.5, -0.5,  0.5)
        glVertex3f(-0.5, -0.5,  0.5)

        # Right face
        glNormal3f(1, 0, 0)
        glVertex3f( 0.5, -0.5, -0.5)
        glVertex3f( 0.5,  0.5, -0.5)
        glVertex3f( 0.5,  0.5,  0.5)
        glVertex3f( 0.5, -0.5,  0.5)

        # Left face
        glNormal3f(-1, 0, 0)
        glVertex3f(-0.5, -0.5, -0.5)
        glVertex3f(-0.5, -0.5,  0.5)
        glVertex3f(-0.5,  0.5,  0.5)
        glVertex3f(-0.5,  0.5, -0.5)

        glEnd()

    def _draw_cylinder(self):
        """Draw a cylinder using quad strips."""
        segments = 16
        glBegin(GL_QUAD_STRIP)
        for i in range(segments + 1):
            angle = 2.0 * math.pi * i / segments
            x = math.cos(angle)
            y = math.sin(angle)

            glNormal3f(x, y, 0)
            glVertex3f(x * 0.5, y * 0.5, -0.5)
            glVertex3f(x * 0.5, y * 0.5,  0.5)
        glEnd()

        # Draw end caps
        glBegin(GL_TRIANGLE_FAN)
        glNormal3f(0, 0, 1)
        glVertex3f(0, 0, 0.5)
        for i in range(segments + 1):
            angle = 2.0 * math.pi * i / segments
            glVertex3f(math.cos(angle) * 0.5, math.sin(angle) * 0.5, 0.5)
        glEnd()

        glBegin(GL_TRIANGLE_FAN)
        glNormal3f(0, 0, -1)
        glVertex3f(0, 0, -0.5)
        for i in range(segments + 1):
            angle = -2.0 * math.pi * i / segments
            glVertex3f(math.cos(angle) * 0.5, math.sin(angle) * 0.5, -0.5)
        glEnd()

    def _draw_cone(self):
        """Draw a cone."""
        segments = 12
        glBegin(GL_TRIANGLE_FAN)
        glNormal3f(0, 0, 1)
        glVertex3f(0, 0, 0.5)  # Tip
        for i in range(segments + 1):
            angle = 2.0 * math.pi * i / segments
            x = math.cos(angle) * 0.5
            y = math.sin(angle) * 0.5
            # Calculate normal for cone surface
            normal_len = math.sqrt(x*x + y*y + 0.25)
            glNormal3f(x/normal_len, y/normal_len, 0.5/normal_len)
            glVertex3f(x, y, -0.5)
        glEnd()

        # Draw base
        glBegin(GL_TRIANGLE_FAN)
        glNormal3f(0, 0, -1)
        glVertex3f(0, 0, -0.5)
        for i in range(segments + 1):
            angle = -2.0 * math.pi * i / segments
            glVertex3f(math.cos(angle) * 0.5, math.sin(angle) * 0.5, -0.5)
        glEnd()

    def _draw_fixed_axes(self):
        """Draw fixed NED frame axes."""
        glDisable(GL_LIGHTING)
        glLineWidth(3.0)

        glBegin(GL_LINES)
        # North axis (X-axis, red) - Forward
        glColor3f(1, 0, 0)
        glVertex3f(0, 0, 0)
        glVertex3f(3, 0, 0)

        # East axis (Y-axis, green) - Right
        glColor3f(0, 1, 0)
        glVertex3f(0, 0, 0)
        glVertex3f(0, 3, 0)

        # Down axis (Z-axis, blue) - Down
        glColor3f(0, 0, 1)
        glVertex3f(0, 0, 0)
        glVertex3f(0, 0, 3)
        glEnd()

    def update_plot(self):
        """Update the 3D display with current vehicle state."""
        self.update()  # Trigger a repaint

    def _draw_body_axes(self):
        """Draw aircraft body frame axes."""
        glDisable(GL_LIGHTING)
        glLineWidth(2.0)

        glBegin(GL_LINES)
        # X-axis (red) - Aircraft longitudinal axis
        glColor3f(1, 0.5, 0.5)
        glVertex3f(0, 0, 0)
        glVertex3f(2, 0, 0)

        # Y-axis (green) - Aircraft lateral axis
        glColor3f(0.5, 1, 0.5)
        glVertex3f(0, 0, 0)
        glVertex3f(0, 2, 0)

        # Z-axis (blue) - Aircraft vertical axis
        glColor3f(0.5, 0.5, 1)
        glVertex3f(0, 0, 0)
        glVertex3f(0, 0, 2)
        glEnd()

        glLineWidth(1.0)
        glEnable(GL_LIGHTING)

    def mousePressEvent(self, event: QMouseEvent):
        """Handle mouse press events."""
        if event.button() == Qt.MouseButton.LeftButton:
            self.mouse_pressed = True
            self.mouse_x = event.position().x()
            self.mouse_y = event.position().y()

    def mouseReleaseEvent(self, event: QMouseEvent):
        """Handle mouse release events."""
        if event.button() == Qt.MouseButton.LeftButton:
            self.mouse_pressed = False

    def mouseMoveEvent(self, event: QMouseEvent):
        """Handle mouse move events for camera rotation."""
        if self.mouse_pressed:
            dx = event.position().x() - self.mouse_x
            dy = event.position().y() - self.mouse_y

            self.camera_rotation_y += dx * 0.5
            self.camera_rotation_x += dy * 0.5

            # Clamp vertical rotation
            if self.camera_rotation_x > 90:
                self.camera_rotation_x = 90
            elif self.camera_rotation_x < -90:
                self.camera_rotation_x = -90

            self.mouse_x = event.position().x()
            self.mouse_y = event.position().y()

            self.update()  # Trigger repaint