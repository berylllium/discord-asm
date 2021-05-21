#pragma once

#include "memory.h"
#include "processor.h"

#include "instructions/mov.h"


class ProgramEnvironment
{
private:

    std::string programCode;
    bool dumpMemory, dumpFull;

    CPU processor;
    MEM memory;

public:
    ProgramEnvironment(std::string programCode, bool dumpMemory, bool dumpFull);
    ~ProgramEnvironment();

public:
    static std::map<std::string, InstructionBase*> instructionClasses;
    std::string consoleBuffer;

    std::string preMemoryDump;
    std::string postMemoryDump;

    bool compile();

    bool run();

    std::string dump_memory();
};