#pragma once

#include <deque>
#include "mymemory.h"
#include "processor.h"
#include "client_tasks.h"
#include "environment_settings.h"

#include "instructions/mov.h"
#include "instructions/cmp.h"
#include "instructions/jne.h"
#include "instructions/add.h"
#include "instructions/call.h"
#include "instructions/ret.h"
#include "instructions/prtc.h"
#include "instructions/jmp.h"


class ProgramEnvironment
{
private:

    CPU processor;
    MEM memory;

public:
    ProgramEnvironment(std::string programCode, bool dumpMemory, bool dumpFull, EnvironmentSettings environmentSettings);
    ~ProgramEnvironment();

public:
    bool dumpMemory, dumpFull;

    std::map<std::string, InstructionBase*> instructionClasses
    {
        { "mov", new Mov() }, { "cmp", new Cmp() }, { "jne", new Jne() },
        { "add", new Add() }, { "call", new Call() }, { "jmp", new Jmp() },
        { "prtc", new Prtc() }, { "ret", new Ret() }
    };
    std::string programCode;

    std::string preMemoryDump;
    std::string postMemoryDump;

    ClientTasks clientTasks;

    uint64_t channelSentFromId;

    bool compile();

    bool run();

    std::string dump_memory();
};