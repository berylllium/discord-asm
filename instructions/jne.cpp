#include "jne.h"

bool Jne::compile(std::string line,
                  MEM& memory,
                  CPU& processor,
                  Word& compilerPointer,
                  std::vector<std::string>& tokens,
                  std::map<std::string, Word>& labelReferences,
                  std::string& consoleBuffer)
{
    memory[compilerPointer++] = CPU::instructionOpCodes[tokens[0]][ADDRESSING_MODES::ABSOLUTE];

    labelReferences[tokens[1]] = compilerPointer;
    compilerPointer += 2;

    return true;
}

bool Jne::run(ADDRESSING_MODES addressingMode,
              MEM& memory,
              CPU& processor,
              std::string& consoleBuffer)
{
    switch (addressingMode)
    {
    case ADDRESSING_MODES::ABSOLUTE:
    {
        Word readWord = processor.read_byte(memory) | (processor.read_byte(memory) << 8);
        if (processor.CF != 1) processor.PC = readWord;
    }break;
    }

    return true;
}
