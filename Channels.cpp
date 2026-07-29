#include "Channels.hpp"

Channel::Channel() : _name("")
{
}

Channel::Channel(const std::string &name) : _name(name)
{
}

Channel::Channel(const Channel &src) : _name(src._name), _users(src._users)
{
}

Channel &Channel::operator=(const Channel &src)
{
	_name = src._name;
	_users = src._users;
	return (*this);
}

Channel::~Channel()
{
}

void Channel::add_user(client &cl)
{
	_users.push_back(&cl);
}

void Channel::add_operator(client &cl)
{
	_operators.push_back(&cl);
}

void Channel::removeOperator(client &cl)
{
	std::vector<client *>::iterator it;
	for (it = _operators.begin(); it != _operators.end(); it++)
	{
		if (*it == &cl)
			_operators.erase(it);
	}
}

void Channel::setInviteOnly(bool value)
{
	_inviteOnly = value;
}

void Channel::setChannelKey(std::string &key)
{
	_channelKey = key;
}

void Channel::setUserLimit(unsigned int u)
{
    _userLimit = u;
}

void Channel::setTopic(std::string &topic)
{
    _topic = topic;
}

bool Channel::isInviteOnly() const
{
	return _inviteOnly;
}

std::string Channel::getChannelKey() const
{
	return _channelKey;
}

unsigned int Channel::getUserLimit() const
{
    return _userLimit;
}

std::string Channel::getTopic() const
{
    return _topic;
}

std::vector<client*> Channel::getUserList() const {
    return _users;
}
std::vector<client*> Channel::getOperatorsList() const {
    return _operators;
}

