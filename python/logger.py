import json
import time
from datetime import datetime
from ctypes import CDLL, c_float

# Загрузка C++ библиотеки
lib = CDLL('./cpp/build/libsysmon.so')
lib.get_cpu_usage.restype = c_float
lib.get_ram_used.restype = c_float

def log_metrics():
    while True:
        metrics = {
            "timestamp": datetime.now().isoformat(),
            "cpu": lib.get_cpu_usage(),
            "ram_used": lib.get_ram_used()
        }
        with open("monitor.log", "a") as f:
            f.write(json.dumps(metrics) + "\n")
        time.sleep(5)

if __name__ == "__main__":
    log_metrics()