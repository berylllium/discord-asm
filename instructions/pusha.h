#pragma once

#include "../instruction_base.h"

#undef ABSOLUTE

class Pusha : public InstructionBase
{
public:

    virtual bool compile(std::string line,
                         MEM& memory,
                         CPU& processor,
                         Word& compilerPointer,
                         std::vector<std::string>& tokens,
                         std::map<std::string, Word>& labelReferences,
                         ClientTasks& clientTasks);
    virtual bool run(ADDRESSING_MODES addressingMode,
                     MEM& memory,
                     CPU& processor,
                     ClientTasks& clientTasks);
};