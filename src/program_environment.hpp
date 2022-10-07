#pragma once

#include <deque>
#include "mymemory.hpp"
#include "processor.hpp"
#include "client_tasks.hpp"
#include "environment_settings.hpp"

#include "instructions/mov.hpp"
#include "instructions/cmp.hpp"
#include "instructions/jne.hpp"
#include "instructions/add.hpp"
#include "instructions/call.hpp"
#include "instructions/ret.hpp"
#include "instructions/prtc.hpp"
#include "instructions/jmp.hpp"
#include "instructions/prtn.hpp"
#include "instructions/jeq.hpp"
#include "instructions/dec.hpp"
#include "instructions/inc.hpp"
#include "instructions/pusha.hpp"
#include "instructions/popa.hpp"

class ProgramEnvironment
{
private:

    CPU processor;
    MEM memory;

public:
    ProgramEnvironment(std::string programCode, uint64_t channelId, bool dumpMemory, bool dumpFull, EnvironmentSettings environmentSettings);
    ~ProgramEnvironment();

public:
    bool dumpMemory, dumpFull;

    std::map<std::string, InstructionBase*> instructionClasses
    {
        { "mov", new Mov() }, { "cmp", new Cmp() }, { "jne", new Jne() },
        { "add", new Add() }, { "call", new Call() }, { "jmp", new Jmp() },
        { "prtc", new Prtc() }, { "ret", new Ret() }, { "prtn", new Prtn() },
        { "jeq", new Jeq() }, { "dec", new Dec() }, { "inc", new Inc() },
        { "pusha", new Pusha() }, { "popa", new Popa() }
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