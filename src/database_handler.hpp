#pragma once

#include "user_settings.hpp"
#include <string>
#include <sstream>
#include <sqlite3.h>

namespace database_handler
{
    static const std::string USER_DATABASE_NAME = "userdata.db";

    static sqlite3* user_database = nullptr;

    bool initialize_database();
    void close_database();

    bool save_user_settings(UserSettings user_settings);
    UserSettings get_user_settings(uint64_t user_id);

    bool save_program_code(std::string code, uint64_t user_id, std::string name);
    std::string get_program_code(uint64_t user_id, std::string name);
}