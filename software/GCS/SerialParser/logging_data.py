from collections import deque
from enum import Enum

class Severity(Enum):
    """ This class holds types of log severities.
    """
    INFO = 1 
    WARNING = 2
    ERR = 3

class LoggingData:
    """ This class holds logs of differing severities.
    """
    def __init__(self, max_logs: int) -> None:
        self.info_logs = deque(maxlen=max_logs)
        self.warn_logs = deque(maxlen=max_logs)
        self.err_logs = deque(maxlen=max_logs)
                
    def add_log(self, string: str, tp: str, severity: Severity) -> None:
        if (severity == Severity.INFO):
            self.info_logs.append(Log(string, tp))
        if (severity == Severity.WARNING):
            self.warn_logs.append(Log(string, tp))
        if (severity == Severity.ERR):
            self.err_logs.append(Log(string, tp))
       
class Log:
    """ This class holds a log.
    """
    def __init__(self, string: str, tp: str) -> None:
        self.log_string = string
        self.tp = tp