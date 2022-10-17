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
                            "dumpFull BOOL," \
                            "runOnPost BOOL"
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

bool database_handler::does_record_in_database_exist(std::string db, std::string column, std::string value, bool value_is_string)
{
    std::stringstream ss;

    ss << "SELECT COUNT(1) FROM " << db << " WHERE " << column << " = " << (value_is_string ? "\"" : "") << value << (value_is_string ? "\"" : "") << ";";

    bool contains = false;

    char* error;
    sqlite3_exec(database_handler::user_database, ss.str().c_str(),
                [](void* pv, int arc, char** argv, char** colName) 
                {
                    bool* ptr = static_cast<bool*>(pv);

                    *ptr = *argv[0] == '1';

                    return 0;
                },
                &contains, &error);
    
    if (error != NULL) {sqlite3_free(error); return false;}
    sqlite3_free(error); 

    return contains;
}

bool database_handler::does_record_in_database_exist(std::string db, uint64_t user_id, std::string column, std::string value, bool value_is_string)
{
    std::stringstream ss;

    ss << "SELECT 1 FROM " << db << " WHERE " << "UserID = " << user_id << " AND " << column << " = " << (value_is_string ? "\"" : "") << value << (value_is_string ? "\"" : "") << ";";

    bool contains = false;

    char* error;
    sqlite3_exec(database_handler::user_database, ss.str().c_str(),
                [](void* pv, int arc, char** argv, char** colName) 
                {
                    bool* ptr = static_cast<bool*>(pv);

                    *ptr = *argv[0] == '1';

                    return 0;
                },
                &contains, &error);
    
    if (error != NULL) {sqlite3_free(error); return false;}
    sqlite3_free(error); 

    return contains;
}

bool database_handler::save_user_settings(UserSettings user_settings)
{
    std::stringstream ss;

    if (database_handler::does_record_in_database_exist("Users", "UserID", std::to_string(user_settings.userID)))
    {
        ss << "UPDATE Users SET " <<
        "dumpMemory = " << user_settings.dumpMemory <<
        ", dumpFull = " << user_settings.dumpFull <<
        ", runOnPost = " << user_settings.run_on_post <<
        " WHERE UserID = " << user_settings.userID << ";";
    }
    else
    {
        ss << "INSERT INTO Users (UserID, dumpMemory, dumpFull, runOnPost) VALUES (" <<
        user_settings.userID << ", " <<
        user_settings.dumpMemory << ", " <<
        user_settings.dumpFull << ", " <<
        user_settings.run_on_post << ");";
    }

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
    
    ss << "SELECT dumpMemory, dumpFull, runOnPost FROM Users WHERE UserID=" << user_id << ";";

    UserSettings settings = {0, false, false, false};

    char* error;
    sqlite3_exec(database_handler::user_database, ss.str().c_str(),
                [](void* pv, int arc, char** argv, char** colName) 
                {
                    UserSettings* ptr = static_cast<UserSettings*>(pv);

                    ptr->dumpMemory = *argv[0] == '1';
                    ptr->dumpFull = *argv[1] == '1';
                    ptr->run_on_post = *argv[2] == '1';

                    return 0;
                },
                &settings, &error);
    
    
    if (error != NULL) {sqlite3_free(error); return settings;}
    sqlite3_free(error); 

    settings.userID = user_id; // Do this so that when an error occurs, the userID within the returned settings object
                               // is set to 0, and it is possible to see an arror ocurred.

    return settings;
}

bool database_handler::save_last_program_code(uint64_t user_id, std::string name)
{
    

    // Check if last exists.
    if (database_handler::does_record_in_database_exist("ProgramCode", user_id, "Name", database_handler::LAST_CODE_NAME, true))
    {
        std::stringstream ss;

        ss << "UPDATE ProgramCode SET Name = '" << name << "' WHERE UserID = " << user_id << " AND Name = '" << database_handler::LAST_CODE_NAME << "';";

        char* error;
        sqlite3_exec(database_handler::user_database, ss.str().c_str(),
                    [](void* nu, int arc, char** argv, char** colName){return 0;}, 0, &error);

        if (error != NULL) {sqlite3_free(error); return false;}
        sqlite3_free(error); 
    }
    else return false;

    return true;
}

bool database_handler::save_program_code(std::string code, uint64_t user_id, std::string name)
{
    std::stringstream ss; 

    // Check if save already exists.
    if (database_handler::does_record_in_database_exist("ProgramCode", user_id, "Name", name, true))
    {
        ss << "UPDATE ProgramCode SET " <<
        "Code = \'" << code << 
        "\' WHERE UserID = " << user_id << " AND Name = \"" << name << "\";";
    }
    else
    {
        ss << "INSERT INTO ProgramCode (UserID, Name, Code) VALUES (" <<
        user_id << ", " <<
        "\"" << name << "\", " <<
        "\'" << code << "\');";
    }

    char* error;
    sqlite3_exec(database_handler::user_database, ss.str().c_str(),
                [](void* nu, int arc, char** argv, char** colName){return 0;}, 0, &error);


    if (error != NULL) {sqlite3_free(error); return false;}
    sqlite3_free(error); 

    return true;
}

bool database_handler::delete_program_code(uint64_t user_id, std::string name)
{
    std::stringstream ss;

    ss << "DELETE FROM ProgramCode WHERE UserID = " << user_id << " AND Name = '" << name << "';";

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

                    *ptr = std::string(argv[0]);

                    return 0;
                },
                &code, &error);
    
     
    if (error != NULL) {sqlite3_free(error); return "";}
    sqlite3_free(error); 

    return code;
}
