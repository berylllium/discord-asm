#include "botclient.h"

BotClient::BotClient(const char* token) : SleepyDiscord::DiscordClient(token, SleepyDiscord::USER_CONTROLED_THREADS)
{

}

void BotClient::onMessage(SleepyDiscord::Message message)
{
    if (message.author.bot) return;
    
    currentChannel = message.channelID;

    std::string messageContent = message.content;

    if (messageContent.find(prefix, 0) == 0) { handleCommands(message); return; }

    if (messageContent.find("```\n&\n", 0) == 0)
    {
        messageContent = messageContent.substr(6, messageContent.length() - 9);

        UserSettings settings = getUserSettings(message.author.ID.number());

        ProgramEnvironment environment(messageContent, settings.dumpMemory, settings.dumpFull);

        std::string fileName = "memoryDump.txt";

        if (environment.compile())
        {
            if (settings.dumpMemory)
            {
                if (static_cast<bool>(std::ifstream(fileName))) std::remove(fileName.c_str());
                std::ofstream outfile(fileName);
                outfile << environment.preMemoryDump;
                outfile.close();

                uploadFile(currentChannel, fileName, "Pre-execution State of Memory:");
            }

            if (environment.run())
            {
                if (settings.dumpMemory)
                {
                    if (static_cast<bool>(std::ifstream(fileName))) std::remove(fileName.c_str());
                    std::ofstream outfile(fileName);
                    outfile << environment.postMemoryDump;
                    outfile.close();

                    uploadFile(currentChannel, fileName, "Post-execution State of Memory:");
                }

                if (!environment.clientTasks.consoleBuffer.empty())
                {
                    sendMessageToChannel(environment.clientTasks.consoleBuffer);
                }
            }
        }
    }
}

void BotClient::sendMessageToChannel(std::string message)
{
    sendMessage(currentChannel, message);
}

bool BotClient::setUserSettings(uint64_t id, UserSettings settings)
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

            lines[i] == lineStream.str();

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

UserSettings BotClient::getUserSettings(uint64_t id)
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
                dumpMemory,
                dumpFull
            };
        }
    }

    return UserSettings
    {
        false,
        false
    };
}

void BotClient::handleCommands(SleepyDiscord::Message message)
{
    std::vector<std::string> tokens = asmutils::split(message.content, ' ');

    uint64_t userId = message.ID.number();

    if (tokens[0].substr(1) == "settings")
    {
        if (tokens.size() < 3) { sendMessageToChannel("User has not provided enough arguments. Please try again."); return; }
        if (tokens[1] == "dumpMemory")
        {
            if (tokens[2] == "true")
            {
                UserSettings settings = getUserSettings(userId);
                settings.dumpMemory = true;
                setUserSettings(userId, settings);
            }
            else if (tokens[2] == "false")
            {
                UserSettings settings = getUserSettings(userId);
                settings.dumpMemory = false;
                setUserSettings(userId, settings);
            }
            else sendMessageToChannel("Third argument must be either 'true' or 'false'.");
        }
        else if (tokens[1] == "dumpFull")
        {
            if (tokens[2] == "true")
            {
                UserSettings settings = getUserSettings(userId);
                settings.dumpFull = true;
                setUserSettings(userId, settings);
            }
            else if (tokens[2] == "false")
            {
                UserSettings settings = getUserSettings(userId);
                settings.dumpFull = false;
                setUserSettings(userId, settings);
            }
            else sendMessageToChannel("Third argument must be either 'true' or 'false'.");
        }
    }
}