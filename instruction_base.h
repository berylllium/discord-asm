#pragma once

#include <map>
#include "processor.h"
#include "memory.h"
#include "definitions.h"
#include "util.h"

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
                         std::string& consoleBuffer) = 0;
    virtual bool run(ADDRESSING_MODES addressingMode,
                     MEM& memory,
                     CPU& processor,
                     std::string& consoleBuffer) = 0;

protected:

};
