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
                            ");" \
                            "CREATE TABLE IF NOT EXISTS ProgramCode(" \
                            "UserID UNSIGNED BIGINT NOT NULL," \
                            "Name TEXT," \
                            "Code TEXT" \
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
    std::stringstream ss;

    ss << "INSERT INTO Users (UserID, dumpMemory, dumpFull) VALUES (" <<
    user_settings.userID << ", " <<
    user_settings.dumpMemory << ", " <<
    user_settings.dumpFull << ");";
    
    char* error;
    sqlite3_exec(database_handler::user_database, ss.str().c_str(),
                [](void* nu, int arc, char** argv, char** colName){return 0;}, 0, &error);

    
    if (error != NULL) {sqlite3_free(error); return false;}
    sqlite3_free(error); 

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
    
    
    if (error != NULL) {sqlite3_free(error); return settings;}
    sqlite3_free(error); 

    settings.userID = user_id; // Do this so that when an error occurs, the userID within the returned settings object
                               // is set to 0, and it is possible to see an arror ocurred.

    return settings;
}

bool database_handler::save_program_code(std::string code, uint64_t user_id, std::string name)
{
    std::stringstream ss;

    ss << "INSERT INTO ProgramCode (UserID, Name, Code) VALUES (" <<
    user_id << ", " <<
    "\"" << name << "\", " <<
    "\"" << code << "\");";

    char* error;
    sqlite3_exec(database_handler::user_database, ss.str().c_str(),
                [](void* nu, int arc, char** argv, char** colName){return 0;}, 0, &error);


    if (error != NULL) {sqlite3_free(error); return false;}
    sqlite3_free(error); 

    return true;
}

std::string database_handler::get_program_code(uint64_t user_id, std::string name)
{
    std::stringstream ss;

    ss << "SELECT Code FROM ProgramCode WHERE UserID=" << user_id << " AND Name=\"" << name << "\";";

    std::string code = "";

    char* error;
    sqlite3_exec(database_handler::user_database, ss.str().c_str(),
                [](void* pv, int arc, char** argv, char** colName) 
                {
                    std::string* ptr = static_cast<std::string*>(pv);

                    *ptr = argv[0];

                    return 0;
                },
                &code, &error);
    
     
    if (error != NULL) {sqlite3_free(error); return "";}
    sqlite3_free(error); 

    return code;
}
