#include "ret.h"

bool Ret::compile(std::string line,
                  MEM& memory,
                  CPU& processor,
                  Word& compilerPointer,
                  std::vector<std::string>& tokens,
                  std::map<std::string, Word>& labelReferences,
                  std::string& consoleBuffer)
{
    memory[compilerPointer++] = CPU::instructionOpCodes[tokens[0]][ADDRESSING_MODES::IMPLICIT];

    return true;
}

bool Ret::run(ADDRESSING_MODES addressingMode,
              MEM& memory,
              CPU& processor,
              std::string& consoleBuffer)
{
    switch (addressingMode)
    {
    case ADDRESSING_MODES::IMPLICIT:
    {
        processor.PC = processor.DI.value;
    }break;
    }

    return true;
}
