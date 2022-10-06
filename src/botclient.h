#pragma once

#include "dpp/dpp.h"
#include "program_environment.h"
#include "definitions.h"
#include "user_settings.h"
#include "database_handler.h"
#include "environment_settings.h"
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