#include "program_environment.h"

std::map<std::string, InstructionBase*> ProgramEnvironment::instructionClasses
{
    { "mov", new Mov() }, { "cmp", new Cmp() }
};

ProgramEnvironment::ProgramEnvironment(std::string programCode, bool dumpMemory, bool dumpFull)
{
    this->dumpMemory = dumpMemory;
    this->programCode = programCode;
    this->dumpFull = dumpFull;

    this->consoleBuffer = "";
}

ProgramEnvironment::~ProgramEnvironment() 
{
    for (auto const& x : instructionClasses)
    {
        delete x.second;
    }
}

bool ProgramEnvironment::compile()
{
    std::map<std::string, Word> labels;
    std::map<std::string, Word> labelReferences;

    Word compilerPointer = 0x200;

    std::vector<std::string> lines;

    { // I hate this so much
        std::stringstream stream(programCode);

        std::string intermediate;

        while (std::getline(stream, intermediate, '\n'))
        {
            lines.push_back(intermediate);
        }
    }

    for (int i = 0; i < lines.size(); i++)
    {
        std::vector<std::string> tokens;

        { // I hate this so much x2
            std::stringstream stream(lines[i]);

            std::string intermediate;

            while (std::getline(stream, intermediate, ' '))
            {
                if (intermediate.empty()) continue;
                if (intermediate.rfind(',') == intermediate.length() - 1) intermediate = intermediate.substr(0, intermediate.length() - 1);
                tokens.push_back(intermediate);
            }
        }

        if (tokens.empty()) continue; // Empty line

    begin_check:

        if (instructionClasses.count(tokens[0])) // Instruction Exists
        {
            if (!instructionClasses[tokens[0]]->compile(lines[i], memory, processor, compilerPointer,
                                                       tokens, labelReferences, consoleBuffer)) return false;
        }
        else // Check for label
        {
            if (tokens[0].rfind(':') == tokens[0].length() - 1) // First token is a label
            {
                if (tokens.size() == 1) // Line is only a token
                {
                    std::string labelName = tokens[0].substr(0, tokens[0].length() - 1);

                    if (labels.count(labelName) == 0) // Label doesnt already exist
                    {
                        labels[labelName] = compilerPointer;
                    }
                    else { consoleBuffer += asmutils::throw_label_already_defined_exception(lines[i], labelName, labels[labelName]); return false; }
                }
                else // Line is token + inline line
                {
                    std::string labelName = tokens[0].substr(0, tokens[0].length() - 1);

                    if (labels.count(labelName) == 0) // Label doesnt already exist
                    {
                        labels[labelName] = compilerPointer;
                    }
                    else { consoleBuffer += asmutils::throw_label_already_defined_exception(lines[i], labelName, labels[labelName]); return false; }

                    tokens.erase(tokens.begin());

                    goto begin_check;
                }
            }
            else // Throw undefined operation exception
            {
                return false;
            }
        }
    }

    preMemoryDump = dump_memory();

    return true;
}

bool ProgramEnvironment::run()
{
    while (processor.PC < MEM::MAX_MEM)
    {
        Byte instruction = processor.read_byte(memory);
        
        for (auto const& x : CPU::instructionOpCodes) // String (name), Map (All addressing modes and op codes available)
        {
            for (auto const& y : CPU::instructionOpCodes[x.first]) // Addressing Mode, Op Code
            {
                if (y.second == instruction)
                {
                    instructionClasses[x.first]->run(y.first, memory, processor);
                }
            }
        }
    }

    postMemoryDump = dump_memory();

    return true;
}

std::string ProgramEnvironment::dump_memory()
{

    std::stringstream ss;
    ss << std::hex;

    // Processor State
    ss << "Processor State:\n    16-bit Registers:   AX = " << std::setw(4) << std::setfill('0') << processor.AX.value << " | " <<
        "BX = " << std::setw(4) << std::setfill('0') << processor.BX.value << " | " <<
        "CX = " << std::setw(4) << std::setfill('0') << processor.CX.value <<
        "\n                        DX = " << std::setw(4) << processor.DX.value << " | " <<
        "SI = " << std::setw(4) << std::setfill('0') << processor.SI.value << " | " <<
        "DI = " << std::setw(4) << std::setfill('0') << processor.DI.value <<
        "\n                        SP = " << std::setw(4) << std::setfill('0') << processor.SP.value << " | " <<
        "BP = " << std::setw(4) << std::setfill('0') << processor.BP.value << " | " <<
        "\n    64-bit Registers:   R8  = " << std::setw(16) << std::setfill('0') << processor.R8 << " | " <<
        "R9  = " << std::setw(16) << std::setfill('0') << processor.R9 <<
        "\n                        R10 = " << std::setw(16) << std::setfill('0') << processor.R10 << " | " <<
        "R11 = " << std::setw(16) << std::setfill('0') << processor.R11 <<
        "\n                        R12 = " << std::setw(16) << std::setfill('0') << processor.R12 << " | " <<
        "R13 = " << std::setw(16) << std::setfill('0') << processor.R13 <<
        "\n                        R14 = " << std::setw(16) << std::setfill('0') << processor.R14 << " | " <<
        "R15 = " << std::setw(16) << std::setfill('0') << processor.R15 << "\n\n";

    ss << "Offset    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n";

    for (int i = 0; i < MEM::MAX_MEM; i++)
    {
        if (i % 16 == 0) ss << "\n" << "0x" << std::setw(4) << std::setfill('0') << i << "    ";
        ss << std::setw(2) << std::setfill('0') << (int)memory[i] << " ";
    }

    return ss.str();
}