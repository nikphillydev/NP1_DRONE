import signalslot
from collections import deque
from enum import Enum

class Severity(Enum):
    """ This class holds the types of log severities.
    """
    INFO = 1 
    WARNING = 2
    ERR = 3

class LoggingData:
    """ This class holds the logs of differing severities.
    """
    def __init__(self, max_logs: int) -> None:
        self.info_logs = deque(maxlen=max_logs)
        self.warn_logs = deque(maxlen=max_logs)
        self.err_logs = deque(maxlen=max_logs)
        
        self.added_log_signal = signalslot.Signal(args=['severity'])
        
    def add_info_log(self, string: str, tp: str) -> None:
        self.info_logs.append(Log(Severity.INFO, string, tp))
        self.added_log_signal.emit(severity=Severity.INFO)
        
    def add_warn_log(self, string: str, tp: str) -> None:
        self.warn_logs.append(Log(Severity.WARNING, string, tp))
        self.added_log_signal.emit(severity=Severity.WARNING)
        
    def add_err_log(self, string: str, tp: str) -> None:
        self.err_logs.append(Log(Severity.ERR, string, tp))
        self.added_log_signal.emit(severity=Severity.ERR)
       
        
class Log:
    """ This class holds a log of a certain severity.
    """
    def __init__(self, severity: Severity, string: str, tp: str) -> None:
        self.severity = severity
        self.log_string = string
        self.tp = tp
        # print(str(severity) + " " + tp + " " + string)