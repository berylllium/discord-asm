#include "botclient.hpp"

BotClient::BotClient(std::string token) : dpp::cluster(token,
dpp::i_default_intents | dpp::i_message_content | dpp::i_guild_messages | dpp::i_guild_integrations) // 
{
    on_ready([this](const dpp::ready_t &e) {on_ready_event(&e);});
    on_message_create([this](const dpp::message_create_t &m) {on_message_create_event(&m);});
    on_slashcommand([this](const dpp::slashcommand_t &e) {on_slashcommand_event(&e); });
    on_message_context_menu([this](const dpp::message_context_menu_t &e) {on_message_context_menu_event(&e);});
    on_form_submit([this](const dpp::form_submit_t &e) {on_form_submit_event(&e);});
    
}

void BotClient::on_ready_event(const dpp::ready_t *ready_event)
{
    bool debugging = true;
    if (dpp::run_once<struct register_bot_commands>() || debugging)
    {
        dpp::slashcommand ctx_menu_save("save program", "Save the selected message as a program.", this->me.id);
        ctx_menu_save.set_type(dpp::ctxm_message);
        
        dpp::slashcommand csettings("settings", "Edit user settings.", this->me.id);
        csettings.add_option(dpp::command_option(dpp::co_boolean, "dump_memory", "Whether the bot will dump memory after compilation & execution."));
        csettings.add_option(dpp::command_option(dpp::co_boolean, "dump_full", "Whether the bot will do a full dump (~240KB) or a minimal dump (~1KB)."));
        csettings.add_option(dpp::command_option(dpp::co_boolean, "run_on_post", "Whether the bot will run program code on post."));

        dpp::slashcommand crun("run", "Run a previously saved program.", this->me.id);
        crun.add_option(dpp::command_option(dpp::co_string, "program_name", "The name of the saved program to run.", true));
        crun.add_option(dpp::command_option(dpp::co_string, "args", "Arguments to pass on to the program."));

        global_command_create(ctx_menu_save);
        global_command_create(csettings);
        global_command_create(crun);
    }
}

const void BotClient::on_message_create_event(const dpp::message_create_t *event)
{
    if (event->msg.author.is_bot()) return;
    
    currentChannel = event->msg.channel_id;

    std::string messageContent = event->msg.content;

    UserSettings settings = database_handler::get_user_settings(event->msg.author.id);

    if (!settings.run_on_post) return;


    if (messageContent.find("```\n&\n", 0) == 0)
    {
        messageContent = messageContent.substr(6, messageContent.length() - 9);

        ProgramEnvironment environment(messageContent, event->msg.channel_id, settings, EnvironmentSettings {});

        run_environment(&environment);
    }
}

void BotClient::send_message_to_channel(dpp::snowflake channelId, std::string content)
{
    message_create(dpp::message(channelId, content));
}

void BotClient::on_slashcommand_event(const dpp::slashcommand_t *event)
{
    dpp::command_value EMPTY_PARAM = dpp::command_value();
    uint64_t user_id = event->command.member.user_id;

    if (event->command.get_command_name() == "settings")
    {
        bool params = false;
        UserSettings u_settings = database_handler::get_user_settings(user_id);
        
        dpp::command_value cv_dump_memory = event->get_parameter("dump_memory");
        dpp::command_value cv_dump_full = event->get_parameter("dump_full");
        dpp::command_value cv_run_on_post = event->get_parameter("run_on_post");

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
        if (cv_run_on_post != EMPTY_PARAM)
        {
            params = true;
            u_settings.run_on_post = std::get<bool>(cv_run_on_post);
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
    else if (event->command.get_command_name() == "run")
    {
        dpp::command_value cv_program_name = event->get_parameter("program_name");
        
        if (cv_program_name == EMPTY_PARAM)
        {
            dpp::message msg = dpp::message("Please provide a program name.");
            msg.set_flags(dpp::message_flags::m_ephemeral);
            event->reply(msg);
            return;
        }

        std::string program_name = std::get<std::string>(cv_program_name);

        if (!database_handler::does_record_in_database_exist("ProgramCode", event->command.member.user_id, "Name", program_name, true))
        {
            dpp::message msg = dpp::message("Provided program does not exist.");
            msg.set_flags(dpp::message_flags::m_ephemeral);
            event->reply(msg);
            return;
        }

        std::string program_code = database_handler::get_program_code(user_id, program_name);

        // Generate environmentsettings

        dpp::command_value cv_args = event->get_parameter("args");
        EnvironmentSettings env_set {};

        if (cv_args != EMPTY_PARAM)
        {
            std::string args = std::get<std::string>(cv_args);

            std::vector<std::string> tokens = asmutils::split(args, ' ', [](std::string t) {
            return !t.empty();
            });

            for (int i = 0; i < tokens.size(); i++)
            {
                if (tokens[i].find("--") == 0) // Argument
                {
                    if (tokens[i].find('=') == std::string::npos)
                    {
                        dpp::message msg = dpp::message("Argument `" + tokens[i] + "` is invalid, as it does not contain an equals (`=`) character.");
                        msg.set_flags(dpp::message_flags::m_ephemeral);
                        event->reply(msg);
                        return;
                    }

                    std::vector<std::string> pair = asmutils::split(tokens[i], '=');

                    if (pair.size() > 2)
                    {
                        dpp::message msg = dpp::message("Argument `" + tokens[i] + "` is invalid, as it contains multiple equals (`=`) characters.");
                        msg.set_flags(dpp::message_flags::m_ephemeral);
                        event->reply(msg);
                        return;
                    }

                    env_set.parameters[pair[0].substr(2, pair[0].length())] = pair[1];
                }
                else if (tokens[i].find("-" == 0)) // Flag set
                {

                }
                else // Error
                {
                    dpp::message msg = dpp::message("Argument `" + tokens[i] + "` is invalid, as it does not start with a single our double hyphen.");
                    msg.set_flags(dpp::message_flags::m_ephemeral);
                    event->reply(msg);
                    return;
                }
            }
        }

        UserSettings user_settings = database_handler::get_user_settings(user_id);

        ProgramEnvironment env(program_code, event->command.channel_id, user_settings, env_set);

        run_environment(&env);
    }
}

void BotClient::on_message_context_menu_event(const dpp::message_context_menu_t *event)
{
    if (event->command.get_command_name() == "save program")
    {
        std::string program_code = event->get_message().content;

        if (program_code.find("```\n&\n", 0) == 0) // Correct program format
        {
            dpp::interaction_modal_response modal("save_program_modal", "Save Program");

            modal.add_component(
                dpp::component()
                .set_label("Program Name")
                .set_id("program_name")
                .set_type(dpp::cot_text)
                .set_placeholder("My Program")
                .set_min_length(1)
                .set_max_length(40)
                .set_text_style(dpp::text_short)
            );

            program_code = program_code.substr(6, program_code.length() - 9);

            if (!database_handler::save_program_code(program_code, event->command.member.user_id, database_handler::LAST_CODE_NAME))
            {
                dpp::message msg = dpp::message("Something went wrong. Please contact a moderator.");
                msg.set_flags(dpp::message_flags::m_ephemeral);

                event->reply(msg);
                return;
            }

            event->dialog(modal);
        }
        else
        {
            dpp::message msg = dpp::message("Message does not follow proper code format.");
            msg.set_flags(dpp::message_flags::m_ephemeral);

            event->reply(msg);
            return;
        }
    }
}

void BotClient::on_form_submit_event(const dpp::form_submit_t *event)
{
    if (event->custom_id == "save_program_modal")
    {
        uint64_t user_id = event->command.member.user_id;
        std::string prgm_name = std::get<std::string>(event->components[0].components[0].value);

        if (database_handler::does_record_in_database_exist("ProgramCode", user_id, "Name", prgm_name, true))
            database_handler::delete_program_code(user_id, prgm_name);

        database_handler::save_last_program_code(user_id, prgm_name);

        dpp::message msg = dpp::message("Sucessfully saved program as `" + prgm_name + "`");
        msg.set_flags(dpp::message_flags::m_ephemeral);
        
        event->reply(msg);
    }
}

void BotClient::run_environment(ProgramEnvironment* environment)
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