#include "Channels.hpp"

Channel::Channel() : _name(""), _inviteOnly(false), _topicRestricted(false), _userLimit(INT_MAX), _isLimited(false)
{
}

Channel::Channel(const std::string &name) : _name(name),  _inviteOnly(false), _topicRestricted(false), _userLimit(INT_MAX), _isLimited(false)
{
}

Channel::Channel(const Channel &src) : _name(src._name), _channelKey(src._channelKey),
	_topic(src._topic), _inviteOnly(src._inviteOnly), _topicRestricted(src._topicRestricted), _userLimit(src._userLimit), _isLimited(src._isLimited),
	_users(src._users), _operators(src._operators)
{
}

Channel &Channel::operator=(const Channel &src)
{
	_name = src._name;
	_channelKey = src._channelKey;
	_topic = src._topic;
	_inviteOnly = src._inviteOnly;
	_topicRestricted = src._topicRestricted;
	_userLimit = src._userLimit;
	_users = src._users;
	_operators = src._operators;
	_isLimited = src._isLimited;
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
		{
			_operators.erase(it);
			return;
		}
	}
}

void Channel::removeUser(client &cl)
{
	std::vector<client *>::iterator it;
	for (it = _users.begin(); it != _users.end(); it++)
	{
		if (*it == &cl)
		{
			_users.erase(it);
			return;
		}
	}
}


void Channel::setInviteOnly(bool value)
{
	_inviteOnly = value;
}

void Channel::setTopicRestricted(bool value)
{
	_topicRestricted = value;
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

void Channel::setIsLimited(bool value)
{
	_isLimited = value;
}

bool Channel::isInviteOnly() const
{
	return _inviteOnly;
}

std::string Channel::getName() const
{
	return _name;
}

bool Channel::isTopicRestricted() const
{
	return _topicRestricted;
}

bool Channel::isLimited() const
{
    return _isLimited;
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

unsigned int Channel::getNumberOfUsers() const
{
    return _users.size();
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

bool Channel::isOperator(client& cl)
{
    for(std::vector<client*>::iterator it = _operators.begin(); it < _operators.end(); it++)
    {
        if(*it == &cl)
            return true;
    }
    return false;
}