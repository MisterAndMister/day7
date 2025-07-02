from ctypes import CDLL, c_float
import time

lib = CDLL('./cpp/build/libsysmon.so')
lib.get_cpu_usage.restype = c_float
lib.get_ram_used.restype = c_float

# Первый вызов CPU (должен вернуть 0)
print(f"CPU (1): {lib.get_cpu_usage():.1f}%")

# Пауза для накопления данных
time.sleep(0.5)

# Второй вызов (должен показать реальную загрузку)
print(f"CPU (2): {lib.get_cpu_usage():.1f}%")

# Проверка RAM
print(f"RAM: {lib.get_ram_used():.1f} MB")