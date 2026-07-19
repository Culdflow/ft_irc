#include "client.hpp"

//CONSTRUCTOR-------------------------------------

client::client()
{
	this->_socketFd = 0;
	this->_name = "";
	this->_nick = "";
	this->_paswdSent = false;
}

client::client(int fd)
{
	this->_socketFd = fd;
	this->_name = "";
	this->_nick = "";
	this->_paswdSent = false;
}

client::client(const client& src)
{
	if (this != &src)
		*this = src;
}

//OPERATORS-----------------------------------

client&	client::operator=(const client& src)
{
	this->_name = src._name;
	this->_nick = src._name;
	this->_socketFd = src._socketFd;
	return (*this);
}

//IS??----------------------------------------

bool	client::isNameSet()const
{
	return (!this->_name.empty());
}

bool	client::isNickSet()const
{
	return (!this->_nick.empty());
}

bool	client::isPaswdSent()const
{
	return(this->_paswdSent);
}

//GETTER----------------------------------------

int		client::getSocketFd()const
{
	return (this->_socketFd);
}

//SETTER----------------------------------------

void	client::setNick(std::string newNick)
{
	this->_nick = newNick;
}

void	client::setName(std::string newName)
{
	this->_name = newName;
}

void	client::setPaswdSent(bool paswdsent)
{
	this->_paswdSent = paswdsent;
}

//DESTRUCTOR-------------------------------------

client::~client()
{}