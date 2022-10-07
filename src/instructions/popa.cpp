#include "popa.hpp"

bool Popa::compile(std::string line,
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

bool Popa::run(ADDRESSING_MODES addressingMode,
               MEM& memory,
               CPU& processor,
               ClientTasks& clientTasks)
{
    switch (addressingMode)
    {
    case ADDRESSING_MODES::IMPLICIT:
    {
        processor.DI.value = processor.pop_stack_word(memory);
        processor.SI.value = processor.pop_stack_word(memory);
        processor.BP.value = processor.pop_stack_word(memory);

        processor.BX.value = processor.pop_stack_word(memory);
        processor.DX.value = processor.pop_stack_word(memory);
        processor.CX.value = processor.pop_stack_word(memory);
        processor.AX.value = processor.pop_stack_word(memory);
    }break;
    case ADDRESSING_MODES::IMMEDIATE:
    case ADDRESSING_MODES::ABSOLUTE:
    case ADDRESSING_MODES::REGISTRY:
    case ADDRESSING_MODES::REGISTRY_POINTER:
    return false;
    }

    return true;
}
