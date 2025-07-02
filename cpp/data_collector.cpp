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

// CPU: возвращает загрузку в процентах (0-100)
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

    unsigned long long total_delta = 0;
    for (size_t i = 0; i < cpu_times.size() && i < prev_cpu_times.size(); ++i) {
        total_delta += cpu_times[i] - prev_cpu_times[i];
    }
    unsigned long long idle_delta = cpu_times.size() > 3 ? (cpu_times[3] - prev_cpu_times[3]) : 0;

    prev_cpu_times = cpu_times;
    if (total_delta == 0) return 0.0f;
    return 100.0f * (1.0f - static_cast<float>(idle_delta) / total_delta);
}

// RAM: возвращает использованную память в MB
static float real_get_ram_used() {
    std::ifstream file("/proc/meminfo");
    std::unordered_map<std::string, float> mem_info;
    std::string line;

    while (std::getline(file, line)) {
        if (line.rfind("MemTotal:", 0) == 0) {
            mem_info["total"] = parse_first_number(line) / 1024.0f;
        } else if (line.rfind("MemFree:", 0) == 0) {
            mem_info["free"] = parse_first_number(line) / 1024.0f;
        } else if (line.rfind("Buffers:", 0) == 0) {
            mem_info["buffers"] = parse_first_number(line) / 1024.0f;
        } else if (line.rfind("Cached:", 0) == 0) {
            mem_info["cached"] = parse_first_number(line) / 1024.0f;
        }
    }

    float total = mem_info["total"];
    float free   = mem_info["free"];
    float buffers = mem_info.count("buffers") ? mem_info["buffers"] : 0.0f;
    float cached  = mem_info.count("cached")  ? mem_info["cached"]  : 0.0f;

    float used = total - free - buffers - cached;
    return used < 0.0f ? 0.0f : used;
}

// Экспортируемые функции для Python (без C++ name-mangling)
extern "C" {
    float get_cpu_usage() {
        return real_get_cpu_usage();
    }

    float get_ram_used() {
        return real_get_ram_used();
    }
}
