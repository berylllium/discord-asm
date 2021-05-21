#include "discord-asm.h"

int main()
{
    std::ifstream infile("token.txt");
    std::stringstream ss;

    ss << infile.rdbuf();

    infile.close();

    BotClient client(ss.str());

    client.run();
}
