#include "popa.h"

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
    case ADDRESSING_MODES::ABSOLUTE:
    {
        processor.DI.value = processor.pull_stack_word(memory);
        processor.SI.value = processor.pull_stack_word(memory);
        processor.SP.value = processor.pull_stack_word(memory);
        processor.BP.value = processor.pull_stack_word(memory);

        processor.BX.value = processor.pull_stack_word(memory);
        processor.DX.value = processor.pull_stack_word(memory);
        processor.CX.value = processor.pull_stack_word(memory);
        processor.AX.value = processor.pull_stack_word(memory);
    }break;
    }

    return true;
}
