#include "botclient.h"

BotClient::BotClient(const char* token) : SleepyDiscord::DiscordClient(token, SleepyDiscord::USER_CONTROLED_THREADS)
{

}

void BotClient::onMessage(SleepyDiscord::Message message)
{
    if (message.author.bot) return;

    std::string messageContent = message.content;

    if (messageContent.find("```\n&\n", 0) == 0)
    {
        messageContent = messageContent.substr(6, messageContent.length() - 9);

        //bool dumpMemory = messageContent.rfind("--dumpMemory") != std::string::npos;
        //bool dumpFull = messageContent.rfind("--dumpMemoryFull") != std::string::npos;

        //if (messageContent.find("-") != std::string::npos)
        //    messageContent = messageContent.substr(0, messageContent.find("-") - 1); // Get space too

        currentChannel = message.channelID;

        ProgramEnvironment environment(messageContent, true, true);

        std::string fileName = "memoryDump.txt";

        if (environment.compile())
        {
            if (static_cast<bool>(std::ifstream(fileName))) std::remove(fileName.c_str());
            std::ofstream outfile(fileName);
            outfile << environment.preMemoryDump;
            outfile.close();

            uploadFile(currentChannel, fileName, "Pre-execution State of Memory:");

            if (environment.run())
            {
                if (static_cast<bool>(std::ifstream(fileName))) std::remove(fileName.c_str());
                std::ofstream outfile(fileName);
                outfile << environment.postMemoryDump;
                outfile.close();

                uploadFile(currentChannel, fileName, "Post-execution State of Memory:");

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