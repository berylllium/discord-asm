#pragma once

#include <deque>
#include "memory.h"
#include "processor.h"
#include "client_tasks.h"

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

    std::string programCode;
    bool dumpMemory, dumpFull;

    CPU processor;
    MEM memory;

public:
    ProgramEnvironment(std::string programCode, bool dumpMemory, bool dumpFull);
    ~ProgramEnvironment();

public:
    std::map<std::string, InstructionBase*> instructionClasses
    {
        { "mov", new Mov() }, { "cmp", new Cmp() }, { "jne", new Jne() },
        { "add", new Add() }, { "call", new Call() }, { "jmp", new Jmp() },
        { "prtc", new Prtc() }, { "ret", new Ret() }
    };

    std::string preMemoryDump;
    std::string postMemoryDump;

    ClientTasks clientTasks;

    bool compile();

    bool run();

    std::string dump_memory();
};