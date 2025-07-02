// sysmon.cpp
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

// Утилитарная функция: чтение первого числа из строки
static float parse_first_number(const std::string& line) {
    std::istringstream iss(line);
    float value;
    iss >> value;
    return value;
}

// Реальная функция CPU: возвращает загрузку в процентах (0-100)
static float real_get_cpu_usage() {
    static std::vector<unsigned long long> prev_cpu_times(10, 0);
    std::ifstream file("/proc/stat");
    std::string line;
    std::getline(file, line);
    std::istringstream iss(line.substr(5));
    std::vector<unsigned long long> cpu_times;
    unsigned long long val;
    while (iss >> val) {
        cpu_times.push_back(val);
    }
    unsigned long long total_time = 0;
    for (size_t i = 0; i < cpu_times.size(); ++i) {
        total_time += cpu_times[i] - prev_cpu_times[i];
    }
    unsigned long long idle_time = cpu_times[3] - prev_cpu_times[3];
    float usage = 100.0f * (1.0f - static_cast<float>(idle_time) / total_time);
    prev_cpu_times = cpu_times;
    return usage;
}

// Реальная функция RAM: возвращает used в MB
static float real_get_ram_used() {
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
    return mem_info["used"];
}

// Экспортируемые C-функции
extern "C" {
    float get_cpu_usage() {
        return real_get_cpu_usage();
    }

    float get_ram_used() {
        return real_get_ram_used();
    }
}
