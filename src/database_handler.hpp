#pragma once
#include "user_settings.hpp"
#include <string>
#include <filesystem>

namespace database_handler
{
    static const std::string USER_DATA_PATH = ".\\userdata";
    static const std::string PROGRAM_SAVES_DIR_NAME = "programsaves";



    static bool saveProgramCode(uint64_t id, std::string fileName, std::string programCode, bool specialSave = false)
    {
        std::stringstream ss;
        ss << USER_DATA_PATH << "\\" << id << "\\" << PROGRAM_SAVES_DIR_NAME;

        std::filesystem::create_directories(ss.str());


        ss << "\\" << (specialSave ? "_" : "") << "programsave_" << fileName << ".txt";

        std::ofstream out(ss.str());
        out << programCode;
        out.close();

        return true;
    }

    static std::string getProgramCode(uint64_t id, std::string fileName)
    {
        std::stringstream ss;
        ss << USER_DATA_PATH << "\\" << id << "\\" << PROGRAM_SAVES_DIR_NAME << "\\programsave_" << fileName << ".txt";
        
        std::ifstream infile(ss.str());
        ss.str(std::string());
        ss << infile.rdbuf();
        infile.close();

        return ss.str();
    }

    static bool setUserSettings(uint64_t id, UserSettings settings)
    {
        std::ifstream infile("db.txt");
        std::stringstream ss;

        ss << infile.rdbuf();

        infile.close();

        std::vector<std::string> lines = asmutils::split(ss.str(), '\n');

        for (long long unsigned int i = 0; i < lines.size(); i++)
        {
            std::vector<std::string> tokens = asmutils::split(lines[i], ' ');

            if (std::stoull(tokens[0]) == id) // User exists in data base
            {
                std::stringstream lineStream;

                lineStream << id << " " << std::to_string(settings.dumpMemory) << " " << std::to_string(settings.dumpFull);

                lines[i] = lineStream.str();

                lineStream.str(std::string());

                std::copy(lines.begin(), lines.end(), std::ostream_iterator<std::string>(lineStream, "\n"));

                std::ofstream out("db.txt");
                out << lineStream.str();;
                out.close();

                return true;
            }
        }

        // User does not exist in data base

        std::stringstream lineStream;

        lineStream << id << " " << std::to_string(settings.dumpMemory) << " " << std::to_string(settings.dumpFull);

        lines.push_back(lineStream.str());

        lineStream.str(std::string());

        std::copy(lines.begin(), lines.end(), std::ostream_iterator<std::string>(lineStream, "\n"));

        std::ofstream out("db.txt");
        out << lineStream.str();;
        out.close();

        return false;
    }

    static UserSettings getUserSettings(uint64_t id)
    {
        std::ifstream infile("db.txt");
        std::stringstream ss;

        ss << infile.rdbuf();

        infile.close();

        std::vector<std::string> lines = asmutils::split(ss.str(), '\n');

        for (long long unsigned int i = 0; i < lines.size(); i++)
        {
            std::vector<std::string> tokens = asmutils::split(lines[i], ' ');

            if (std::stoull(tokens[0]) == id)
            {
                bool dumpMemory, dumpFull;;
                std::istringstream(tokens[1]) >> dumpMemory;
                std::istringstream(tokens[2]) >> dumpFull;

                return UserSettings
                {
                    id,
                    dumpMemory,
                    dumpFull
                };
            }
        }

        return UserSettings
        {
            id,
            false,
            false
        };
    }
}