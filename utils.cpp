#include "utils.hpp"

bool cmd_exist(const std::string& cmd)
{
    static const std::string commands[] = {
        "JOIN",
        "PART",
        "PRIVMSG",
        "KICK",
        "INVITE",
        "TOPIC",
        "MODE",
        "QUIT"
    };

    for (size_t i = 0; i < 8; i++)
    {
        if (cmd == commands[i])
            return true;
    }
    return false;
}