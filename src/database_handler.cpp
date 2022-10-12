#include "database_handler.hpp"

bool database_handler::initialize_database()
{
    int errored = sqlite3_open(database_handler::USER_DATABASE_NAME.c_str(), 
                               &database_handler::user_database);
    if (errored) return false;

    // Create User table if not exist.
    std::string create_t = "CREATE TABLE IF NOT EXISTS Users(" \
                            "UserID UNSIGNED BIGINT NOT NULL," \
                            "dumpMemory BOOL," \
                            "dumpFull BOOL" \
                            ");";
    
    sqlite3_exec(database_handler::user_database, create_t.c_str(),
                [](void* nu, int arc, char** argv, char** colName){return 0;}, 0, NULL);
    
    return true;
}

void database_handler::close_database()
{
    sqlite3_close(database_handler::user_database);
}

bool database_handler::save_user_settings(UserSettings user_settings)
{
    std::string exec = "INSERT INTO Users (UserID, dumpMemory, dumpFull)" \
                        "VALUES (" + std::to_string(user_settings.userID) + ", " +
                        std::to_string(user_settings.dumpMemory) + ", " \
                        ;
    std::stringstream ss;

    ss << "INSERT INTO Users (UserID, dumpMemory, dumpFull) VALUES (" <<
    user_settings.userID << ", " <<
    user_settings.dumpMemory << ", " <<
    user_settings.dumpFull << ");";
    
    char* error;
    sqlite3_exec(database_handler::user_database, ss.str().c_str(),
                [](void* nu, int arc, char** argv, char** colName){return 0;}, 0, &error);

    sqlite3_free(error); 
    if (error != NULL) return false;
    

    return true;
}

UserSettings database_handler::get_user_settings(uint64_t user_id)
{
    std::stringstream ss;
    
    ss << "SELECT dumpMemory, dumpFull FROM Users WHERE UserID=" << user_id << ";";

    UserSettings settings = {0, false, false};

    char* error;
    sqlite3_exec(database_handler::user_database, ss.str().c_str(),
                [](void* pv, int arc, char** argv, char** colName) 
                {
                    UserSettings* ptr = static_cast<UserSettings*>(pv);

                    ptr->dumpMemory = *argv[0] == '1';
                    ptr->dumpFull = *argv[1] == '1';

                    return 0;
                },
                &settings, &error);
    
    sqlite3_free(error); 
    if (error != NULL) return settings;

    settings.userID = user_id; // Do this so that when an error occurs, the userID within the returned settings object
                               // is set to 0, and it is possible to see an arror ocurred.

    return settings;
}
