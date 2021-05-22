#include "call.h"

bool Call::compile(std::string line,
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

bool Call::run(ADDRESSING_MODES addressingMode,
               MEM& memory,
               CPU& processor,
               ClientTasks& clientTasks)
{
    switch (addressingMode)
    {
    case ADDRESSING_MODES::ABSOLUTE:
    {
        Word address = processor.read_byte(memory) | (processor.read_byte(memory) << 8);
        processor.DI.value = processor.PC;
        processor.PC = address;
    }break;
    }

    return true;
}
