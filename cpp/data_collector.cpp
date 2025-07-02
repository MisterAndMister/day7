#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

// Утилитарная функция: чтение первого числа из строки
float parse_first_number(const std::string& line) {
    std::istringstream iss(line);
    float value;
    iss >> value;
    return value;
}

// CPU: возвращает загрузку в процентах (0-100)
float get_cpu_usage() {
    static std::vector<unsigned long long> prev_cpu_times(10, 0);
    
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line); // Первая строка - суммарная статистика CPU
    
    std::istringstream iss(line.substr(5)); // Пропускаем "cpu  "
    std::vector<unsigned long long> cpu_times;
    unsigned long long val;
    while (iss >> val) {
        cpu_times.push_back(val);
    }
    
    // Расчет загрузки (формула из man proc)
    unsigned long long total_time = 0;
    for (size_t i = 0; i < cpu_times.size(); ++i) {
        total_time += cpu_times[i] - prev_cpu_times[i];
    }
    
    unsigned long long idle_time = cpu_times[3] - prev_cpu_times[3];
    float usage = 100.0f * (1.0f - static_cast<float>(idle_time) / total_time);
    
    prev_cpu_times = cpu_times;
    return usage;
}

// RAM: возвращает used и free в MB
std::unordered_map<std::string, float> get_memory_info() {
    std::ifstream file("/proc/meminfo");
    std::unordered_map<std::string, float> mem_info;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.find("MemTotal:") != std::string::npos) {
            mem_info["total"] = parse_first_number(line) / 1024;
        } else if (line.find("MemFree:") != std::string::npos) {
            mem_info["free"] = parse_first_number(line) / 1024;
        } else if (line.find("Buffers:") != std::string::npos) {
            mem_info["buffers"] = parse_first_number(line) / 1024;
        } else if (line.find("Cached:") != std::string::npos) {
            mem_info["cached"] = parse_first_number(line) / 1024;
        }
    }
    
    mem_info["used"] = mem_info["total"] - mem_info["free"] - mem_info["buffers"] - mem_info["cached"];
    return mem_info;
}

// Диски: возвращает read/write в kB/s
std::unordered_map<std::string, float> get_disk_io() {
    static std::unordered_map<std::string, std::pair<unsigned long, unsigned long>> prev_io;
    std::unordered_map<std::string, float> result;
    
    std::ifstream file("/proc/diskstats");
    std::string line;
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        
        while (iss >> token) {
            tokens.push_back(token);
        }
        
        if (tokens.size() < 14) continue;
        
        std::string disk_name = tokens[2];
        unsigned long reads = std::stoul(tokens[5]);
        unsigned long writes = std::stoul(tokens[9]);
        
        if (prev_io.find(disk_name) != prev_io.end()) {
            auto [prev_reads, prev_writes] = prev_io[disk_name];
            result[disk_name + "_read"] = (reads - prev_reads) / 2.0f;  // Предполагаем 2 сек между вызовами
            result[disk_name + "_write"] = (writes - prev_writes) / 2.0f;
        }
        
        prev_io[disk_name] = {reads, writes};
    }
    
    return result;
}

// Сеть: возвращает in/out трафик в kB/s
std::unordered_map<std::string, float> get_network_usage() {
    static std::unordered_map<std::string, std::pair<unsigned long, unsigned long>> prev_net;
    std::unordered_map<std::string, float> result;
    
    std::ifstream file("/proc/net/dev");
    std::string line;
    
    // Пропускаем первые 2 строки (заголовки)
    std::getline(file, line);
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string iface;
        iss >> iface;
        iface = iface.substr(0, iface.size() - 1); // Убираем двоеточие
        
        unsigned long rx_bytes, tx_bytes;
        for (int i = 0; i < 8; ++i) iss >> rx_bytes; // Пропускаем ненужные поля
        iss >> tx_bytes;
        
        if (prev_net.find(iface) != prev_net.end()) {
            auto [prev_rx, prev_tx] = prev_net[iface];
            result[iface + "_in"] = (rx_bytes - prev_rx) / 2048.0f;  // kB за 2 сек
            result[iface + "_out"] = (tx_bytes - prev_tx) / 2048.0f;
        }
        
        prev_net[iface] = {rx_bytes, tx_bytes};
    }
    
    return result;
}

// Главная функция сбора всех метрик
std::unordered_map<std::string, float> get_metrics() {
    std::unordered_map<std::string, float> metrics;
    
    // CPU
    metrics["cpu_usage"] = get_cpu_usage();
    
    // RAM
    auto mem_info = get_memory_info();
    metrics["ram_total"] = mem_info["total"];
    metrics["ram_used"] = mem_info["used"];
    metrics["ram_free"] = mem_info["free"];
    
    // Диски (берём только sda для примера)
    auto disk_io = get_disk_io();
    metrics["disk_read"] = disk_io.count("sda_read") ? disk_io["sda_read"] : 0;
    metrics["disk_write"] = disk_io.count("sda_write") ? disk_io["sda_write"] : 0;
    
    // Сеть (берём eth0 или wlan0)
    auto net_usage = get_network_usage();
    std::string iface = net_usage.count("eth0_in") ? "eth0" : 
                       (net_usage.count("wlan0_in") ? "wlan0" : "");
    if (!iface.empty()) {
        metrics["net_in"] = net_usage[iface + "_in"];
        metrics["net_out"] = net_usage[iface + "_out"];
    }
    
    return metrics;
}