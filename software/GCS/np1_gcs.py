import tkinter as tk
from tkinter import ttk
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.animation import FuncAnimation

from SerialParser.serial_parser import SerialParserThread
from SensorData.sensor_data import SensorData
from LoggingData.logging_data import LoggingData, Severity
from Plots.plots import AccelerometerPlot, GyroscopePlot


def main():
    # Configuration
    usb_port = "/dev/ttyACM0"
    max_saved_samples = 400
    max_saved_logs = 30
    
    sensor_data = SensorData(max_samples=max_saved_samples)
    logging_data = LoggingData(max_logs=max_saved_logs)
    
    serial_parser = SerialParserThread(usb_port, sensor_data, logging_data)
    serial_parser.start()
    
    app = Application(sensor_data, logging_data)
    app.mainloop()
    
    serial_parser.join()
        
        
class Application(tk.Tk):
    def __init__(self, sensor_data: SensorData, logging_data: LoggingData) -> None:
        super().__init__()
        self.title("NP1 Ground Control Station")
        
        self.logging_data = logging_data
        self.sensor_data = sensor_data
        
        # Create 6 frames in a 2x3 grid
        n, m = 2, 3
        self.frames = [[None for col in range(m)] for row in range(n)]
        for row in range(n):
            for col in range(m):
                frame = tk.Frame(self, borderwidth=5, relief="groove")
                frame.grid(row=row, column=col, sticky="nsew")
                self.frames[row][col] = frame
        
        # Make the grid expand with the window size
        for row in range(n):
            self.grid_rowconfigure(row, weight=1)
        for col in range(m):
            self.grid_columnconfigure(col, weight=1)           
        
        # Add accelerometer plot
        self.acc_plot = AccelerometerPlot(sensor_data.acc_samples)
        self.acc_canvas = FigureCanvasTkAgg(self.acc_plot.figure, master=self.frames[0][0])
        self.acc_canvas.draw()
        self.acc_canvas.get_tk_widget().pack(expand=True, fill=tk.BOTH) 
        self.acc_ani = FuncAnimation(self.acc_plot.figure, self.update_plot, interval = 5, save_count=10, fargs=(self.acc_plot, self.acc_canvas))
        
        # Add gyroscope plot
        self.gyro_plot = GyroscopePlot(sensor_data.gyro_samples)
        self.gyro_canvas = FigureCanvasTkAgg(self.gyro_plot.figure, master=self.frames[0][1])
        self.gyro_canvas.draw()
        self.gyro_canvas.get_tk_widget().pack(expand=True, fill=tk.BOTH) 
        self.gyro_ani = FuncAnimation(self.gyro_plot.figure, self.update_plot, interval = 5, save_count=10, fargs=(self.gyro_plot, self.gyro_canvas))
        
        # Temperature text
        temp_label = tk.Label(self.frames[0][2], text="Temperature (°C): ")
        temp_label.grid(row=0, column=0, padx=5, pady=5) 
        self.temperature_text = tk.Text(self.frames[0][2], height=1, width=10)
        self.temperature_text.grid(row=0, column=1, padx=5, pady=5) 
        self.update_temperature()
        
        # Add INFO logging output
        info_label = tk.Label(self.frames[1][0], text=f"INFO Logs")
        info_label.pack(anchor="nw", padx=5, pady=5)
        self.info_log_text = tk.Text(self.frames[1][0], wrap="word", height=12)
        self.info_log_text.pack(expand=True, fill=tk.BOTH)
        info_scrollbar = tk.Scrollbar(self.frames[1][0], command=self.info_log_text.yview)
        info_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.info_log_text.config(yscrollcommand=info_scrollbar.set)
        
        # Add WARN logging output
        warn_label = tk.Label(self.frames[1][1], text=f"WARN Logs")
        warn_label.pack(anchor="nw", padx=5, pady=5)
        self.warn_log_text = tk.Text(self.frames[1][1], wrap="word", height=12)
        self.warn_log_text.pack(expand=True, fill=tk.BOTH)
        warn_scrollbar = tk.Scrollbar(self.frames[1][1], command=self.warn_log_text.yview)
        warn_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.warn_log_text.config(yscrollcommand=warn_scrollbar.set)
        
        # Add ERROR logging output
        err_label = tk.Label(self.frames[1][2], text=f"ERROR Logs")
        err_label.pack(anchor="nw", padx=5, pady=5)
        self.err_log_text = tk.Text(self.frames[1][2], wrap="word", height=12)
        self.err_log_text.pack(expand=True, fill=tk.BOTH)
        err_scrollbar = tk.Scrollbar(self.frames[1][2], command=self.err_log_text.yview)
        err_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        self.err_log_text.config(yscrollcommand=err_scrollbar.set)
        
        # Connect update logs to signal
        self.logging_data.added_log_signal.connect(self.update_logs)
        
        self.paused = False
        self.pause_button = ttk.Button(self, text="Pause", command=self.toggle_pause)
        self.pause_button.grid(row=n, column=0, columnspan=m, sticky="ew")
        
        self.quit_button = ttk.Button(self, text="Quit", command=self.destroy)
        self.quit_button.grid(row=n+1, column=0, columnspan=m, sticky="ew")
    
    def toggle_pause(self):
        self.paused = not self.paused
    
    def update_plot(self, i, plot, canvas):
        if (self.paused):
            return
        plot.update_plot()
        canvas.draw_idle()
        
    def update_temperature(self):
        self.temperature_text.delete(1.0, tk.END)
        self.temperature_text.insert(tk.END, self.sensor_data.temperature_samples.temp_data[-1])
        self.after(100, self.update_temperature)
        
    def update_logs(self, severity: int, **kwargs):
        if (self.paused):
            return
        
        if (severity == Severity.INFO):
            info_log = self.logging_data.info_logs[-1]
            string = info_log.tp + ": " + info_log.log_string + "\n"
            self.info_log_text.insert(tk.END, string)
            self.info_log_text.see(tk.END)
        if (severity == Severity.WARNING):
            warn_log = self.logging_data.warn_logs[-1]
            string = warn_log.tp + ": " + warn_log.log_string + "\n"
            self.warn_log_text.insert(tk.END, string)
            self.warn_log_text.see(tk.END)
        if (severity == Severity.ERR):
            err_log = self.logging_data.err_logs[-1]
            string = err_log.tp + ": " + err_log.log_string + "\n"
            self.err_log_text.insert(tk.END, string)
            self.err_log_text.see(tk.END)

if __name__ == "__main__":
    main()

