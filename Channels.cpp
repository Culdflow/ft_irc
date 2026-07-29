#include "Channels.hpp"

Channel::Channel() : _name("") {}

Channel::Channel(const std::string& name) : _name(name) {}

Channel::Channel(const Channel& src) : _name(src._name), _users(src._users) {}

Channel& Channel::operator=(const Channel& src)
{
    _name  = src._name;
    _users = src._users;
    return *this;
}

Channel::~Channel() {}

void Channel::add_user(client& cl)
{
    _users.push_back(&cl);
}

void Channel::add_operator(client& cl)
{
    _operators.push_back(&cl);
}

void Channel::broadcast(client& cl, Message& msg)
{
    if(user_present(cl) == true)
    {
        for(std::vector<client*>::iterator it = _users.begin(); it < _users.end(); it++)
        {
            if(*it == &cl)
                continue ;

            std::string message = ":" + cl.getRealName() + "!" + cl.getRealName() + "@ircserv PRIVMSG " + _name + " :";
            for (std::vector<std::string>::iterator it = msg.params.begin() + 1; it != msg.params.end(); it++)
                message = message + *it + " ";
            message = message + "\r\n";
            send((*it)->getSocketFd(), message.c_str(), message.size(), 0);
        }
    }
    else
    {
        std::string mess = ":ircserv 442  ERR_NOTONCHANNEL";
        send(cl.getSocketFd(), mess.c_str(), mess.size(), 0);;
    }
}

bool Channel::user_present(client& cl)
{
    for(std::vector<client*>::iterator it = _users.begin(); it < _users.end(); it++)
    {
        if(*it == &cl)
            return true;
    }
    return false;
}