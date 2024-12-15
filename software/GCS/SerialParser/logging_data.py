from collections import deque
from enum import Enum

class Severity(Enum):
    """ This class holds types of log severities.
    """
    INFO = 1 
    CRITICAL = 2
    ERROR = 3

class LoggingData:
    """ This class holds logs of differing severities.
    """
    def __init__(self, max_logs: int) -> None:
        self.info_logs = deque(maxlen=max_logs)
        self.critical_logs = deque(maxlen=max_logs)
        self.error_logs = deque(maxlen=max_logs)
                
    def add_log(self, string: str, tp: str, severity: Severity) -> None:
        if (severity == Severity.INFO):
            self.info_logs.append(Log(string, tp))
        if (severity == Severity.CRITICAL):
            self.critical_logs.append(Log(string, tp))
        if (severity == Severity.ERROR):
            self.error_logs.append(Log(string, tp))
       
class Log:
    """ This class holds a log.
    """
    def __init__(self, string: str, tp: str) -> None:
        self.log_string = string
        self.tp = tp