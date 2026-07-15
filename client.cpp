#include "client.hpp"

client::client(int fd)
{
	this->_socketFd = fd;
	this->_name = "";
	this->_nick = "";
}

client::client(const client& src)
{
	if (this != &src)
		*this = src;
}

client&	client::operator=(const client& src)
{
	this->_name = src._name;
	this->_nick = src._name;
	this->_socketFd = src._socketFd;
	return (*this);
}

bool	client::isNameSet()const
{
	return (!this->_name.empty());
}

bool	client::isNickSet()const
{
	return (!this->_nick.empty());
}

client::~client()
{}