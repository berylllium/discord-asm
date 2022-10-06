#pragma once

#include <vector>
#include <utility>

struct EnvironmentSettings
{
	std::vector<std::pair<uint8_t, uint64_t>> preloadedRegisters;
};