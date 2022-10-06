#pragma once

#include "dpp/dpp.h"
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

    const void onMessage(const dpp::message_create_t *message);

    void sendMessageToChannel(dpp::snowflake channelId, std::string content);


private:
    dpp::snowflake currentChannel;

    void handleCommands(const dpp::message *message);

    void runEnvironment(ProgramEnvironment *environment, UserSettings userSettings);
};