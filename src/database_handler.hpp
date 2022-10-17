#pragma once

#include "user_settings.hpp"
#include <string>
#include <sstream>
#include <sqlite3.h>

namespace database_handler
{
    static const std::string USER_DATABASE_NAME = "userdata.db";
    static const std::string LAST_CODE_NAME = "_last";

    static sqlite3* user_database = nullptr;

    bool initialize_database();
    void close_database();

    bool does_record_in_database_exist(std::string db, std::string column, std::string value, bool value_is_string = false);
    bool does_record_in_database_exist(std::string db, uint64_t user_id, std::string column, std::string value, bool value_is_string = false);

    bool save_user_settings(UserSettings user_settings);
    UserSettings get_user_settings(uint64_t user_id);

    bool save_last_program_code(uint64_t user_id, std::string name);
    bool save_program_code(std::string code, uint64_t user_id, std::string name);
    bool delete_program_code(uint64_t user_id, std::string name);
    std::string get_program_code(uint64_t user_id, std::string name);
}