#include "ft_irc.hpp"

bool cmd_exist(const std::string& cmd)
{
    static const std::string commands[] = {
        "JOIN",
        "PRIVMSG",
        "KICK",
        "INVITE",
        "TOPIC",
        "MODE",
        "QUIT",
        "PASS",
        "USER",
        "NICK",
        "PING",
        "PONG",
    };

    for (size_t i = 0; i < 12; i++)
    {
        if (cmd == commands[i])
            return true;
    }
    return false;
}