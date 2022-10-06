#include "botclient.h"

BotClient::BotClient(std::string token) : dpp::cluster(token,
dpp::i_default_intents | dpp::i_message_content)
{
    on_message_create([this](const dpp::message_create_t &m) {
        onMessage(&m);
        
        });
}

const void BotClient::onMessage(const dpp::message_create_t *event)
{
    if (event->msg.author.is_bot()) return;
    
    currentChannel = event->msg.channel_id;

    std::string messageContent = event->msg.content;


    if (messageContent.find(prefix, 0) == 0) { handleCommands(&event->msg); return; }

    if (messageContent.find("```\n&\n", 0) == 0)
    {
        messageContent = messageContent.substr(6, messageContent.length() - 9);

        UserSettings settings = database_handler::getUserSettings(event->msg.author.id);

        ProgramEnvironment environment(messageContent, settings.dumpMemory, settings.dumpFull, EnvironmentSettings {});

        runEnvironment(&environment, settings);
    }
}

void BotClient::sendMessageToChannel(dpp::snowflake channelId, std::string content)
{
    message_create(dpp::message(channelId, content));
}

void BotClient::handleCommands(const dpp::message *message)
{
    std::vector<std::string> tokens = asmutils::split(message->content, ' ');

    uint64_t userId = message->author.id;

    if (tokens[0].substr(1) == "settings")
    {
        if (tokens.size() < 3) { sendMessageToChannel(message->channel_id, "User has not provided enough arguments. Please try again."); return; }
        if (tokens[1] == "dumpMemory")
        {
            if (tokens[2] == "true")
            {
                UserSettings settings = database_handler::getUserSettings(userId);
                settings.dumpMemory = true;
                database_handler::setUserSettings(userId, settings);
            }
            else if (tokens[2] == "false")
            {
                UserSettings settings = database_handler::getUserSettings(userId);
                settings.dumpMemory = false;
                database_handler::setUserSettings(userId, settings);
            }
            else sendMessageToChannel(message->channel_id, "Third argument must be either 'true' or 'false'.");
        }
        else if (tokens[1] == "dumpFull")
        {
            if (tokens[2] == "true")
            {
                UserSettings settings = database_handler::getUserSettings(userId);
                settings.dumpFull = true;
                database_handler::setUserSettings(userId, settings);
            }
            else if (tokens[2] == "false")
            {
                UserSettings settings = database_handler::getUserSettings(userId);
                settings.dumpFull = false;
                database_handler::setUserSettings(userId, settings);
            }
            else sendMessageToChannel(message->channel_id, "Third argument must be either 'true' or 'false'.");
        }
    }
    else if (tokens[0].substr(1) == "savelast")
    {
        if (tokens.size() <= 1) { sendMessageToChannel(message->channel_id, "Must provide save name after command."); return; }

        std::stringstream ss; // Path to programsaves dir
        ss << database_handler::USER_DATA_PATH << "\\" << userId << "\\" << database_handler::PROGRAM_SAVES_DIR_NAME;


        std::ifstream src(ss.str() + "\\_programsave_last.txt");
        std::ofstream dst(ss.str() + "\\programsave_" + tokens[1] + ".txt");

        dst << src.rdbuf();

        dst.close();
        src.close();
    }
    else if (tokens[0].substr(1) == "runsaved")
    {
        if (tokens.size() <= 1) { sendMessageToChannel(message->channel_id, "Must provide save name after command."); return; }

        EnvironmentSettings envSettings;

        // Check for register preloads
        if (tokens.size() > 2 && (tokens.size() - 2) % 2 == 0) // Valid amount of parameters
        {
            for (int i = 2; i - 2 < (tokens.size() - 2) / 2; i += 2) // Loop through every pair
            {
                // Check if valid register
                if (CPU::registerEncoding.count(tokens[i]) > 0)
                {
                    try
                    {
                        envSettings.preloadedRegisters.push_back(std::pair(CPU::registerEncoding[tokens[i]], std::stoull(tokens[i+1])));
                    } catch (std::invalid_argument e)
                    {
                        sendMessageToChannel(message->channel_id, "Provided register preload value is not a valid number.");
                        return;
                    } catch (std::out_of_range e)
                    {
                        sendMessageToChannel(message->channel_id, "Provided register preload value is out of range.");
                        return;
                    }
                } 
            }
        }

        UserSettings settings = database_handler::getUserSettings(message->author.id);

        ProgramEnvironment environment(database_handler::getProgramCode(userId, tokens[1]), settings.dumpMemory, settings.dumpFull, envSettings);

        runEnvironment(&environment, settings);
    }
}

void BotClient::runEnvironment(ProgramEnvironment* environment, UserSettings userSettings)
{
    std::string fileName = "./memoryDump.txt";

    if (environment->compile())
    {
        if (environment->dumpMemory)
        {
            if (static_cast<bool>(std::ifstream(fileName))) std::remove(fileName.c_str());
            std::ofstream outfile(fileName);
            outfile << environment->preMemoryDump;
            outfile.close();

            dpp::message msg(environment->channelSentFromId, "Pre-execution State of Memory:"); 
            msg.set_file_content(dpp::utility::read_file(fileName));
            msg.set_filename(fileName);

            if (environment->run())
            {
                database_handler::saveProgramCode(userSettings.userID, "last", environment->programCode, true);

                if (userSettings.dumpMemory)
                {
                    if (static_cast<bool>(std::ifstream(fileName))) std::remove(fileName.c_str());
                    std::ofstream outfile(fileName);
                    outfile << environment->postMemoryDump;
                    outfile.close();

                    dpp::message msg(environment->channelSentFromId, "Post-execution State of Memory:"); 
                    msg.set_file_content(dpp::utility::read_file(fileName));
                    msg.set_filename(fileName);
                }

                if (!environment->clientTasks.consoleBuffer.empty())
                {
                    sendMessageToChannel(environment->channelSentFromId, environment->clientTasks.consoleBuffer);
                }
            }
            else sendMessageToChannel(environment->channelSentFromId, "Runtime Error:\n" + environment->clientTasks.consoleBuffer);
        }
    }
    else sendMessageToChannel(environment->channelSentFromId, "Compilation Error:\n" + environment->clientTasks.consoleBuffer);
}