#pragma once

#include "sleepy_discord/sleepy_discord.h"
#include "program_environment.h"
#include "definitions.h"
#include "user_settings.h"

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

    bool setUserSettings(uint64_t id, UserSettings settings);
    UserSettings getUserSettings(uint64_t id);

    void handleCommands(SleepyDiscord::Message message);

};