#include "program_environment.hpp"

ProgramEnvironment::ProgramEnvironment(std::string programCode, uint64_t channelId, UserSettings uSettings, EnvironmentSettings environmentSettings)
{
    this->environment_settings = environmentSettings;
    this->dumpMemory = uSettings.dumpMemory;
    this->dumpFull = uSettings.dumpFull;
    this->programCode = programCode;
    this->channelSentFromId = channelId;

    for (int i = 0; i < environmentSettings.preloaded_registers.size(); i++) {
        processor.set_register_value(environmentSettings.preloaded_registers[i].first, environmentSettings.preloaded_registers[i].second);
    }
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

    std::vector<std::string> lines = asmutils::split(programCode, '\n');

    for (int i = 0; i < lines.size(); i++)
    {
        std::vector<std::string> tokens = asmutils::split(lines[i], ' ', [](std::string& s) -> bool {
            if (s.empty()) return false;
            if (s.rfind(',') == s.length() - 1) s = s.substr(0, s.length() - 1);
            return true;
        });

        if (tokens.empty()) continue; // Empty line

        // Check and replace any template placeholders
        for (int j = 0; j < tokens.size(); j++)
        {
            int hyphen_pos = tokens[j].find('-');
            int start_bracket_pos = tokens[j].find('{');
            int end_bracket_pos = tokens[j].find("}");

            if (hyphen_pos != std::string::npos && start_bracket_pos != std::string::npos && end_bracket_pos != std::string::npos) // Placeholder characters exist.
            {
                if (start_bracket_pos - hyphen_pos == 1 && end_bracket_pos > start_bracket_pos) // Valid parameter placeholder.
                {
                    std::string placeholder_name = tokens[j].substr(start_bracket_pos + 1, end_bracket_pos - start_bracket_pos - 1);

                    if (environment_settings.parameters.contains(placeholder_name))
                    {
                        std::string repl = environment_settings.parameters[placeholder_name];
                        tokens[j].replace(hyphen_pos, placeholder_name.length() + 3, repl);
                    }
                    else
                    {
                        clientTasks.consoleBuffer += asmutils::make_undefined_parameter_placeholder_exception(lines[i], tokens[j]);
                        return false;
                    }
                }
                else
                {
                    clientTasks.consoleBuffer += asmutils::make_invalid_parameter_placeholder_exception(lines[i], tokens[j]);
                    return false;
                }
            }
        }

    begin_check:

        if (instructionClasses.count(tokens[0])) // Instruction Exists
        {
            if (!instructionClasses[tokens[0]]->compile(lines[i], memory, processor, compilerPointer,
                                                       tokens, labelReferences, clientTasks)) return false;
        } // VVV Check if instruction is pseudo op
        else if (tokens[0] == "db")
        {
            for (int j = 1; j < tokens.size(); j++) // Things to write
            {
                if (tokens[j].find('"') == 0 && tokens[j].rfind('"') == tokens[j].size() - 1) // String
                {
                    std::string insideString = tokens[j].substr(1, tokens[j].size() - 2);
                    const char* charArray = insideString.c_str();

                    for (int k = 0; k < tokens[j].length() - 2; k++)
                    {
                        memory[compilerPointer++] = charArray[k];
                    }
                }
                else if (asmutils::valid_hex_string(tokens[j])) // Byte literal
                {
                    if (asmutils::get_bits(std::stoi(tokens[j], nullptr, 16)) <= 8)
                    {
                        memory[compilerPointer++] = std::stoi(tokens[j], nullptr, 16);
                    }
                    else { clientTasks.consoleBuffer += asmutils::make_possible_overflow_exception(lines[i], asmutils::get_bits(std::stoi(tokens[j], nullptr, 16)), 8); return false; }
                }
            }
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
                    else { clientTasks.consoleBuffer += asmutils::make_label_already_defined_exception(lines[i], labelName, labels[labelName]); return false; }
                }
                else // Line is token + inline line
                {
                    std::string labelName = tokens[0].substr(0, tokens[0].length() - 1);

                    if (labels.count(labelName) == 0) // Label doesnt already exist
                    {
                        labels[labelName] = compilerPointer;
                    }
                    else { clientTasks.consoleBuffer += asmutils::make_label_already_defined_exception(lines[i], labelName, labels[labelName]); return false; }

                    tokens.erase(tokens.begin());

                    goto begin_check;
                }
            }
            else // Throw undefined operation exception
            {
                clientTasks.consoleBuffer += asmutils::make_undefined_instruction_exception(lines[i], tokens[0]);
                return false;
            }
        }
    }

    // Link labels
    for (auto const& x : labelReferences)
    {
        if (labels.count(x.first) > 0) // Label exists
        {
            memory[x.second] = (Byte)(labels[x.first] & 0x00FF);
            memory[x.second + 1] = (Byte)(labels[x.first] >> 8);
        }
        else { clientTasks.consoleBuffer += asmutils::make_label_not_found_exception(x.second, x.first); return false; }
    }

    if (dumpMemory) preMemoryDump = dump_memory();

    return true;
}

bool ProgramEnvironment::run()
{
    while (processor.PC < MEM::MAX_MEM)
    {
    start_run_loop:

        Byte instruction = processor.read_byte(memory);

        if (instruction == 0x00) break;
        
        for (auto const& x : CPU::instructionOpCodes) // String (name), Map (All addressing modes and op codes available)
        {
            for (auto const& y : CPU::instructionOpCodes[x.first]) // Addressing Mode, Op Code
            {
                if (y.second == instruction)
                {
                    instructionClasses[x.first]->run(y.first, memory, processor, clientTasks);
                    goto start_run_loop; // Have to do this to break out of 2 for loops
                }
            }
        }
    }

    if (dumpMemory) postMemoryDump = dump_memory();

    return true;
}

std::string ProgramEnvironment::dump_memory()
{
    std::stringstream ss;
    ss << std::hex;

    // Processor State
    if (dumpFull)
    {
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
    }

    ss << "Offset    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n";

    for (int i = 0; i < MEM::MAX_MEM; i++)
    {
        if (i % 16 == 0) ss << "\n" << "0x" << std::setw(4) << std::setfill('0') << i << "    ";
        ss << std::setw(2) << std::setfill('0') << (int)memory[i] << " ";
    }

    std::string memoryDump = ss.str();

    if (!dumpFull)
    {
        ss.str(std::string()); // Clear string stream
        ss << "Offset    00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n\n";
        std::deque<std::string> lines = asmutils::split_deque(memoryDump, '\n');

        lines.pop_front(); // Remove header
        lines.pop_front(); // Remove empty line

        std::string lastLine;
        bool first = true;

        for (int i = 0; i < lines.size(); i++)
        {
            std::string currentLine = lines[i].substr(10);

            if (currentLine != lastLine)
            {
                ss << lines[i] << "\n";
                lastLine = currentLine;
                first = true;
            }
            else
            {
                if (first)
                {
                    ss << "*\n";
                    first = false;
                }
            }
        }

        memoryDump = ss.str();
    }

    return memoryDump;
}