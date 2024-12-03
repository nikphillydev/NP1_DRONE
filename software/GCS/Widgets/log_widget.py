from PyQt6.QtWidgets import QWidget, QGroupBox, QVBoxLayout, QTextEdit

class LogWidget(QWidget):
    def __init__(self, title: str) -> None:
        super().__init__()
        
        # Create the Group Box with the title
        group_box = QGroupBox(title)
        group_box.setStyleSheet("QGroupBox { font-weight: bold; font-size: 14pt; color: darkblue; }")
        
        # Create a layout for the group box
        layout = QVBoxLayout()

        # Create the QTextEdit widget (scrollable text box)
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)  # Set the text box as read-only
        # self.text_edit.setLineWrapMode(QTextEdit.LineWrapMode.NoWrap)  # Optional: Disable word wrap
        
        # Add the text box to the layout
        layout.addWidget(self.text_edit)

        # Set the layout for the group box
        group_box.setLayout(layout)

        # Set the layout for the widget
        widget_layout = QVBoxLayout()
        widget_layout.addWidget(group_box)
        self.setLayout(widget_layout)
    
    def append_text(self, text: str):
        """Append a new string to the text box"""
        self.text_edit.append(text)