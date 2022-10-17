#pragma once

#include <dpp/dpp.h>
#include "program_environment.hpp"
#include "definitions.hpp"
#include "user_settings.hpp"
#include "database_handler.hpp"
#include "environment_settings.hpp"
#include <utility>

class BotClient : public dpp::cluster
{
public:
    std::string prefix = ",";
    BotClient(std::string token);

    void on_ready_event(const dpp::ready_t *ready_event);
    const void on_message_create_event(const dpp::message_create_t *message);

    void send_message_to_channel(dpp::snowflake channelId, std::string content);


private:
    dpp::snowflake currentChannel;

    void on_slashcommand_event(const dpp::slashcommand_t *event);
    void on_message_context_menu_event(const dpp::message_context_menu_t *event);
    void on_form_submit_event(const dpp::form_submit_t *event);

    void run_environment(ProgramEnvironment *environment);
};