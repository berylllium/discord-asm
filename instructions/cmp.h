#pragma once

#include "../instruction_base.h"

#undef ABSOLUTE

class Cmp : public InstructionBase
{
public:

    virtual bool compile(std::string line,
                         MEM& memory,
                         CPU& processor,
                         Word& compilerPointer,
                         std::vector<std::string>& tokens,
                         std::map<std::string, Word>& labelReferences,
                         std::string& consoleBuffer);
    virtual bool run(ADDRESSING_MODES addressingMode,
                     MEM& memory,
                     CPU& processor);
};
