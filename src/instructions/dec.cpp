#include "dec.hpp"

bool Dec::compile(std::string line,
                  MEM& memory,
                  CPU& processor,
                  Word& compilerPointer,
                  std::vector<std::string>& tokens,
                  std::map<std::string, Word>& labelReferences,
                  ClientTasks& clientTasks)
{
    
    if (CPU::registerEncoding.count(tokens[1]))
    {
        memory[compilerPointer++] = CPU::instructionOpCodes[tokens[0]][ADDRESSING_MODES::ABSOLUTE];
        memory[compilerPointer++] = CPU::registerEncoding[tokens[1]];
    }
    else { clientTasks.consoleBuffer += asmutils::make_undefined_registry_exception(line, tokens[1]); return false; }

    return true;
}

bool Dec::run(ADDRESSING_MODES addressingMode,
              MEM& memory,
              CPU& processor,
              ClientTasks& clientTasks)
{
    switch (addressingMode)
    {
    case ADDRESSING_MODES::ABSOLUTE:
    {
        Byte destinationEncoding = processor.read_byte(memory);
        processor.set_register_value(destinationEncoding, processor.get_register_value(destinationEncoding) - 1);
    }break;
    case ADDRESSING_MODES::IMMEDIATE:
    case ADDRESSING_MODES::IMPLICIT:
    case ADDRESSING_MODES::REGISTRY:
    case ADDRESSING_MODES::REGISTRY_POINTER:
    return false;
    }

    return true;
}
