#pragma once

#include <vector>
#include <string>
#include "definitions.hpp"

struct ClientTasks
{
    std::string consoleBuffer;

    std::vector<uint64_t> usersToBan;
};
