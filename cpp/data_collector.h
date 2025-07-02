#pragma once
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

float parse_first_number(const std::string& line);
float get_cpu_usage();
std::unordered_map<std::string, float> get_memory_info();
std::unordered_map<std::string, float> get_disk_io();
std::unordered_map<std::string, float> get_network_usage();
std::unordered_map<std::string, float> get_metrics();