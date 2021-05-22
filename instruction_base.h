#pragma once

#include <map>
#include "processor.h"
#include "memory.h"
#include "definitions.h"
#include "util.h"
#include "client_tasks.h"

class BotClient;
struct CPU;

class InstructionBase
{
public:

    InstructionBase() {}

public:  

    virtual bool compile(std::string line,
                         MEM& memory,
                         CPU& processor,
                         Word& compilerPointer,
                         std::vector<std::string>& tokens,
                         std::map<std::string, Word>& labelReferences,
                         ClientTasks& clientTasks) = 0;
    virtual bool run(ADDRESSING_MODES addressingMode,
                     MEM& memory,
                     CPU& processor,
                     ClientTasks& clientTasks) = 0;

protected:

};
