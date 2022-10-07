#include "pusha.hpp"

bool Pusha::compile(std::string line,
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

bool Pusha::run(ADDRESSING_MODES addressingMode,
                MEM& memory,
                CPU& processor,
                ClientTasks& clientTasks)
{
    switch (addressingMode)
    {
    case ADDRESSING_MODES::IMPLICIT:
    {
        processor.push_stack(processor.AX.value, memory);
        processor.push_stack(processor.CX.value, memory);
        processor.push_stack(processor.DX.value, memory);
        processor.push_stack(processor.BX.value, memory);

        processor.push_stack(processor.BP.value, memory);
        processor.push_stack(processor.SI.value, memory);
        processor.push_stack(processor.DI.value, memory);
    }break;
    case ADDRESSING_MODES::IMMEDIATE:
    case ADDRESSING_MODES::ABSOLUTE:
    case ADDRESSING_MODES::REGISTRY:
    case ADDRESSING_MODES::REGISTRY_POINTER:
    return false;
    }

    return true;
}
