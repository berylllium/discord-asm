﻿#include "discord-asm.hpp"

int main()
{
    database_handler::initialize_database();

    std::ifstream infile("./token.txt");
    std::stringstream ss;

    ss << infile.rdbuf();

    infile.close();

    BotClient client(ss.str());

    client.start(dpp::st_wait);

    database_handler::close_database();

    return 0;
}
