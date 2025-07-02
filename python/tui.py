import curses
from ctypes import CDLL, c_float
import os
import time

lib_path = os.path.abspath("cpp/build/libsysmon.so")
try:
    lib = CDLL(lib_path)
    lib.get_cpu_usage.restype = c_float
    lib.get_ram_used.restype = c_float
    lib.get_cpu_usage.argtypes = []
    lib.get_ram_used.argtypes = []
except Exception as e:
    print(f"Error loading library: {e}")
    exit(1)

def main(stdscr):
    curses.curs_set(0)  # Скрыть курсор
    stdscr.nodelay(1)   # Неблокирующий ввод
    
    while True:
        stdscr.clear()
        
        try:
            cpu = lib.get_cpu_usage()
            ram = lib.get_ram_used()
            
            if cpu < 0 or cpu > 100:
                raise ValueError(f"Invalid CPU value: {cpu}")
            if ram < 0:
                raise ValueError(f"Invalid RAM value: {ram}")
                
        except Exception as e:
            stdscr.addstr(0, 0, f"Error: {str(e)}")
            stdscr.refresh()
            time.sleep(1)
            continue
        
        stdscr.addstr(0, 0, "System Monitor (Q to quit)")
        stdscr.addstr(2, 0, f"CPU Usage: {cpu:.1f}%")
        stdscr.addstr(3, 0, f"RAM Used: {ram:.1f} MB")
        stdscr.addstr(5, 0, "Debug Info:")
        stdscr.addstr(6, 0, f"- Library: {lib_path}")
        stdscr.addstr(7, 0, f"- Last update: {time.strftime('%H:%M:%S')}")
        
        if stdscr.getch() == ord('q'):
            break
            
        stdscr.refresh()
        time.sleep(1)

if __name__ == "__main__":
    curses.wrapper(main)