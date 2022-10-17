#pragma once

#include <map>
#include <string>
#include <vector>
#include <cstdint>

struct EnvironmentSettings
{
	std::map<std::string, std::string> parameters;
	std::vector<std::pair<uint8_t, uint64_t>> preloaded_registers;
};