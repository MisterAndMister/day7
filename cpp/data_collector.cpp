#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>

static float parse_first_number(const std::string& line) {
    std::istringstream iss(line);
    float value;
    iss >> value;
    return value;
}

static bool starts_with(const std::string& str, const std::string& prefix) {
    return str.rfind(prefix, 0) == 0;
}

static float real_get_cpu_usage() {
    static bool first_call = true;
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

    if (first_call) {
        prev_cpu_times = cpu_times;
        first_call = false;
        return 0.0f;  
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

static float real_get_ram_used() {
    std::ifstream file("/proc/meminfo");
    if (!file.is_open()) return -1.0f;  // Ошибка чтения
    
    float total = 0.0f, free = 0.0f, buffers = 0.0f, cached = 0.0f;
    std::string line;
    
    while (std::getline(file, line)) {
        if (starts_with(line, "MemTotal:")) {
            total = parse_first_number(line) / 1024.0f;
        } else if (starts_with(line, "MemFree:")) {
            free = parse_first_number(line) / 1024.0f;
        } else if (starts_with(line, "Buffers:")) {
            buffers = parse_first_number(line) / 1024.0f;
        } else if (starts_with(line, "Cached:")) {
            cached = parse_first_number(line) / 1024.0f;
        }
    }
    
    float used = total - free - buffers - cached;
    return (used < 0.0f) ? 0.0f : used;
}

extern "C" {
    float get_cpu_usage() {
        return real_get_cpu_usage();
    }

    float get_ram_used() {
        return real_get_ram_used();
    }
}