#pragma once

#include "sleepy_discord/sleepy_discord.h"
#include "program_environment.h"
#include "definitions.h"
#include "user_settings.h"
#include "database_handler.h"
#include "environment_settings.h"
#include <utility>

class BotClient : public SleepyDiscord::DiscordClient
{
public:
    using SleepyDiscord::DiscordClient::DiscordClient;

    std::string prefix = ",";
    BotClient(const char* token);

    void onMessage(SleepyDiscord::Message message) override;

    void sendMessageToChannel(std::string message);


private:
    SleepyDiscord::Snowflake<SleepyDiscord::Channel> currentChannel;

    void handleCommands(SleepyDiscord::Message *message);

    void runEnvironment(ProgramEnvironment *environment, UserSettings userSettings);
};