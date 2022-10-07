#include "prtn.hpp"

bool Prtn::compile(std::string line,
                   MEM& memory,
                   CPU& processor,
                   Word& compilerPointer,
                   std::vector<std::string>& tokens,
                   std::map<std::string, Word>& labelReferences,
                   ClientTasks& clientTasks)
{
    if (CPU::registerEncoding.count(tokens[1]) > 0) // Value is another register
    {
        memory[compilerPointer++] = CPU::instructionOpCodes[tokens[0]][ADDRESSING_MODES::REGISTRY];
        memory[compilerPointer++] = CPU::registerEncoding[tokens[1]];
    }
    else if ((tokens[1].find('[') == 0) && (tokens[1].rfind(']') == tokens[1].length() - 1)) // Value is a pointer register
    {
        memory[compilerPointer++] = CPU::instructionOpCodes[tokens[0]][ADDRESSING_MODES::REGISTRY_POINTER];
        memory[compilerPointer++] = CPU::registerEncoding[tokens[1].substr(1, tokens[1].length() - 2)];
    }
    else if (asmutils::valid_hex_string(tokens[1])) // Value is a number
    {
        int value = std::stoull(tokens[1], nullptr, 16);
        int valueBits = asmutils::get_bits(value);
        if (valueBits <= 8)
        {
            memory[compilerPointer++] = CPU::instructionOpCodes[tokens[0]][ADDRESSING_MODES::IMMEDIATE];
            memory[compilerPointer++] = std::stoull(tokens[1], nullptr, 16);
        }
        else // Throw overflow
        {
            clientTasks.consoleBuffer += asmutils::make_possible_overflow_exception(line, valueBits, 8);
            return false;
        }
    }

    return true;
}

bool Prtn::run(ADDRESSING_MODES addressingMode,
               MEM& memory,
               CPU& processor,
               ClientTasks& clientTasks)
{
    switch (addressingMode)
    {
    case ADDRESSING_MODES::IMMEDIATE:
    {
        Byte character = processor.read_byte(memory);
        clientTasks.consoleBuffer += std::to_string(character);
    }break;
    case ADDRESSING_MODES::REGISTRY:
    {
        Byte sourceEncoding = processor.read_byte(memory);

        clientTasks.consoleBuffer += std::to_string(processor.get_register_value(sourceEncoding));
    }break;
    case ADDRESSING_MODES::REGISTRY_POINTER:
    {
        //Byte sourceEncoding = processor.read_byte(memory);

        Byte registerWithPointer = processor.read_byte(memory);

        clientTasks.consoleBuffer += std::to_string(memory[processor.get_register_value(registerWithPointer)]);
    }break;
    case ADDRESSING_MODES::ABSOLUTE:
    case ADDRESSING_MODES::IMPLICIT:
    return false;
    }

    return true;
}
