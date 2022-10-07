#include "add.hpp"

bool Add::compile(std::string line,
                  MEM& memory,
                  CPU& processor,
                  Word& compilerPointer,
                  std::vector<std::string>& tokens,
                  std::map<std::string, Word>& labelReferences,
                  ClientTasks& clientTasks)
{
    if (CPU::registerEncoding.count(tokens[1])) // Destination exists
    {
        if (asmutils::valid_hex_string(tokens[2])) // Value is a number
        {
            int regBits = asmutils::get_register_size(CPU::registerEncoding[tokens[1]]);
            int valueBits = asmutils::get_bits(std::stoull(tokens[2], nullptr, 16));
            if (regBits >= valueBits)
            {
                memory[compilerPointer++] = CPU::instructionOpCodes[tokens[0]][ADDRESSING_MODES::IMMEDIATE];
                memory[compilerPointer++] = CPU::registerEncoding[tokens[1]];

                std::vector<Byte> bytes = asmutils::int_to_little_endian_byte_array(std::stoull(tokens[2], nullptr, 16));

                for (int i = 0; i < bytes.size(); i++)
                {
                    memory[compilerPointer++] = bytes[i];
                }

                float remainingBytes = regBits / 8 - bytes.size();

                if (remainingBytes / 8.0f != 0.0f)
                {
                    while (remainingBytes > 0)
                    {
                        memory[compilerPointer++] = 0x00;
                        remainingBytes--;
                    }
                }
            }
            else // Throw overflow
            {
                clientTasks.consoleBuffer += asmutils::make_possible_overflow_exception(line, asmutils::get_bits(std::stoull(tokens[2], nullptr, 16)), asmutils::get_register_size(CPU::registerEncoding[tokens[1]]));
                return false;
            }
        }
        else if (CPU::registerEncoding.count(tokens[2])) // Value is another register
        {
            int regBits = asmutils::get_register_size(CPU::registerEncoding[tokens[1]]);
            int valueBits = asmutils::get_register_size(CPU::registerEncoding[tokens[2]]);

            if (regBits >= valueBits)
            {
                memory[compilerPointer++] = CPU::instructionOpCodes[tokens[0]][ADDRESSING_MODES::REGISTRY];
                memory[compilerPointer++] = CPU::registerEncoding[tokens[1]];
                memory[compilerPointer++] = CPU::registerEncoding[tokens[2]];
            }
            else { clientTasks.consoleBuffer += asmutils::make_possible_overflow_exception(line, valueBits, regBits); return false; }
        }
    }

    return true;
}

bool Add::run(ADDRESSING_MODES addressingMode,
              MEM& memory,
              CPU& processor,
              ClientTasks& clientTasks)
{
    switch (addressingMode)
    {
    case ADDRESSING_MODES::IMMEDIATE:
    {
        Byte destinationEncoding = processor.read_byte(memory);

        std::vector<Byte> bytes;

        for (int i = 0; i < asmutils::get_register_size(destinationEncoding) / 8; i++)
        {
            bytes.push_back(processor.read_byte(memory));
        }

        uint64_t value = 0;
        memcpy(&value, &bytes[0], asmutils::get_register_size(destinationEncoding) / 8);

        processor.set_register_value(destinationEncoding, processor.get_register_value(destinationEncoding) + value);
    }break;
    case ADDRESSING_MODES::REGISTRY:
    {
        Byte destinationEncoding = processor.read_byte(memory);
        Byte valueEncoding = processor.read_byte(memory);

        processor.set_register_value(destinationEncoding, processor.get_register_value(destinationEncoding) + processor.get_register_value(valueEncoding));
    }break;
    case ADDRESSING_MODES::ABSOLUTE:
    case ADDRESSING_MODES::IMPLICIT:
    case ADDRESSING_MODES::REGISTRY_POINTER:
    return false;
    }

    return true;
}
