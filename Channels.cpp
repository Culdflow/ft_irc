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
