#pragma once

#include <map>
#include "processor.hpp"
#include "mymemory.hpp"
#include "definitions.hpp"
#include "util.hpp"
#include "client_tasks.hpp"

#pragma warning(disable : 4244)

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
