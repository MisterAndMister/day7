import curses
import time
from ctypes import CDLL, c_float

# Настройка C++ библиотеки
lib = CDLL('./cpp/build/libsysmon.so')
lib.get_cpu_usage.restype = c_float
lib.get_ram_used.restype = c_float

def draw_tui(stdscr):
    stdscr.nodelay(True)
    while True:
        stdscr.clear()
        
        # Получаем метрики
        cpu = lib.get_cpu_usage()
        ram = lib.get_ram_used()
        
        # Рисуем интерфейс
        stdscr.addstr(0, 0, "System Monitor (q to quit)")
        stdscr.addstr(2, 0, f"CPU:  [{cpu:5.1f}%] {'|' * int(cpu/5)}")
        stdscr.addstr(3, 0, f"RAM:  [{ram:5.1f}MB]")
        
        stdscr.refresh()
        
        # Выход по 'q'
        if stdscr.getch() == ord('q'):
            break
        
        time.sleep(1)

if __name__ == "__main__":
    curses.wrapper(draw_tui)
