#include "jmp.h"

bool Jmp::compile(std::string line,
                  MEM& memory,
                  CPU& processor,
                  Word& compilerPointer,
                  std::vector<std::string>& tokens,
                  std::map<std::string, Word>& labelReferences,
                  ClientTasks& clientTasks)
{
    memory[compilerPointer++] = CPU::instructionOpCodes[tokens[0]][ADDRESSING_MODES::ABSOLUTE];

    labelReferences[tokens[1]] = compilerPointer;
    compilerPointer += 2;

    return true;
}

bool Jmp::run(ADDRESSING_MODES addressingMode,
              MEM& memory,
              CPU& processor,
              ClientTasks& clientTasks)
{
    switch (addressingMode)
    {
    case ADDRESSING_MODES::ABSOLUTE:
    {
        processor.PC = processor.read_byte(memory) | (processor.read_byte(memory) << 8);
    }break;
    case ADDRESSING_MODES::IMMEDIATE:
    case ADDRESSING_MODES::IMPLICIT:
    case ADDRESSING_MODES::REGISTRY:
    case ADDRESSING_MODES::REGISTRY_POINTER:
    return false;
    }

    return true;
}
