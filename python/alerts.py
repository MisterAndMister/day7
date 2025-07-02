import psutil  # Для альтернативного получения метрик, если C++ недоступен

def check_alerts(cpu_threshold=90, ram_threshold=10):
    cpu = psutil.cpu_percent()
    ram = 100 - psutil.virtual_memory().percent
    
    alerts = []
    if cpu > cpu_threshold:
        alerts.append(f"CPU превышен {cpu_threshold}% ({cpu:.1f}%)")
    if ram < ram_threshold:
        alerts.append(f"Осталось мало RAM: {ram:.1f}%")
    
    if alerts:
        print("\033[91mALERT:\033[0m " + " | ".join(alerts))

if __name__ == "__main__":
    check_alerts()
