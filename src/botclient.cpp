#include "botclient.hpp"

BotClient::BotClient(std::string token) : dpp::cluster(token,
dpp::i_default_intents | dpp::i_message_content | dpp::i_guild_messages)
{
    on_ready([this](const dpp::ready_t &e) {on_ready_event(&e); });
    on_message_create([this](const dpp::message_create_t &m) {on_message_create_event(&m);});
    on_slashcommand([this](const dpp::slashcommand_t &e) {on_slashcommand_event(&e); });
}

void BotClient::on_ready_event(const dpp::ready_t *ready_event)
{
    bool debugging = true;
    if (dpp::run_once<struct register_bot_commands>() || debugging)
    {
        dpp::slashcommand csettings("settings", "Edit user settings.", this->me.id);
        csettings.add_option(dpp::command_option(dpp::co_boolean, "dump_memory", "Whether the bot will dump memory after compilation & execution."));
        csettings.add_option(dpp::command_option(dpp::co_boolean, "dump_full", "Whether the bot will do a full dump (~240KB) or a minimal dump (~1KB)."));

        global_command_create(csettings);
    }
}

const void BotClient::on_message_create_event(const dpp::message_create_t *event)
{
    if (event->msg.author.is_bot()) return;
    
    currentChannel = event->msg.channel_id;

    std::string messageContent = event->msg.content;


    if (messageContent.find("```\n&\n", 0) == 0)
    {
        messageContent = messageContent.substr(6, messageContent.length() - 9);

        UserSettings settings = database_handler::get_user_settings(event->msg.author.id);

        ProgramEnvironment environment(messageContent, event->msg.channel_id, settings, EnvironmentSettings {});

        run_environment(&environment, settings);
    }
}

void BotClient::send_message_to_channel(dpp::snowflake channelId, std::string content)
{
    message_create(dpp::message(channelId, content));
}

void BotClient::on_slashcommand_event(const dpp::slashcommand_t *event)
{
    dpp::command_value EMPTY_PARAM = dpp::command_value();

    if (event->command.get_command_name() == "settings")
    {
        bool params = false;
        UserSettings u_settings = database_handler::get_user_settings(event->command.member.user_id);
        
        dpp::command_value cv_dump_memory = event->get_parameter("dump_memory");
        dpp::command_value cv_dump_full = event->get_parameter("dump_full");

        if (cv_dump_memory != EMPTY_PARAM) 
        {
            params = true;
            u_settings.dumpMemory = std::get<bool>(cv_dump_memory);
        }
        if (cv_dump_full != EMPTY_PARAM)
        {
            params = true;
            u_settings.dumpFull = std::get<bool>(cv_dump_full);
        }
        
        if (params)
        {
            bool s = database_handler::save_user_settings(u_settings);

            dpp::message msg = s ? dpp::message("Sucessfuly saved settings.") : dpp::message("Something went wrong. Please contact a moderator.");
            msg.set_flags(dpp::message_flags::m_ephemeral);
            event->reply(msg);
        }
        else
        {
            dpp::message msg = dpp::message("Currently unimplemented.");
            msg.set_flags(dpp::message_flags::m_ephemeral);
            event->reply(msg);
        }

        return;
    }
}

void BotClient::run_environment(ProgramEnvironment* environment, UserSettings userSettings)
{
    std::string fileName = "./memoryDump.txt";

    if (environment->compile())
    {
        if (environment->dumpMemory)
        {
            {
                dpp::message msg(environment->channelSentFromId, "Pre-execution State of Memory:"); 
                msg.set_file_content(environment->preMemoryDump);
                msg.set_filename(fileName);

                message_create(msg);
            }

            if (environment->run())
            {
                //database_handler::saveProgramCode(userSettings.userID, "last", environment->programCode, true);

                if (environment->dumpMemory)
                {
                    {
                        dpp::message msg(environment->channelSentFromId, "Post-execution State of Memory:"); 
                        msg.set_file_content(environment->postMemoryDump);
                        msg.set_filename(fileName);

                        message_create(msg);
                    }
                }

                if (!environment->clientTasks.consoleBuffer.empty())
                {
                    send_message_to_channel(environment->channelSentFromId, environment->clientTasks.consoleBuffer);
                }
            }
            else send_message_to_channel(environment->channelSentFromId, "Runtime Error:\n" + environment->clientTasks.consoleBuffer);
        }
    }
    else send_message_to_channel(environment->channelSentFromId, "Compilation Error:\n" + environment->clientTasks.consoleBuffer);
}