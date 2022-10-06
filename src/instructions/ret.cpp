#include "ret.hpp"

bool Ret::compile(std::string line,
                  MEM& memory,
                  CPU& processor,
                  Word& compilerPointer,
                  std::vector<std::string>& tokens,
                  std::map<std::string, Word>& labelReferences,
                  ClientTasks& clientTasks)
{
    memory[compilerPointer++] = CPU::instructionOpCodes[tokens[0]][ADDRESSING_MODES::IMPLICIT];

    return true;
}

bool Ret::run(ADDRESSING_MODES addressingMode,
              MEM& memory,
              CPU& processor,
              ClientTasks& clientTasks)
{
    switch (addressingMode)
    {
    case ADDRESSING_MODES::IMPLICIT:
    {
        processor.PC = processor.DI.value;
    }break;
    case ADDRESSING_MODES::ABSOLUTE:
    case ADDRESSING_MODES::REGISTRY:
    case ADDRESSING_MODES::REGISTRY_POINTER:
    return false;
    }

    return true;
}
