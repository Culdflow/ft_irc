#include "Client.hpp"

//CONSTRUCTOR-------------------------------------

client::client()
{
	this->_socketFd = 0;
	this->_name = "";
	this->_nick = "";
	this->_paswdCorrect = false;
}

client::client(int fd)
{
	this->_socketFd = fd;
	this->_name = "";
	this->_nick = "";
	this->_paswdCorrect = false;
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
	this->_nick = src._nick;
	this->_socketFd = src._socketFd;
	this->_paswdCorrect = src._paswdCorrect;
	this->_inputBuf = src._inputBuf;
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

bool	client::isPaswdCorrect() const
{
	return(this->_paswdCorrect);
}

//GETTER----------------------------------------

int		client::getSocketFd()const
{
	return (this->_socketFd);
}

std::string& client::getInputBuf()
{
    return (this->_inputBuf);
}

const std::string& client::getNick() const {
	return _nick;
}
const std::string& client::getName() const {
	return _name;
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

void	client::setPaswdCorrect(bool paswdCorrect)
{
	this->_paswdCorrect = paswdCorrect;
}

//DESTRUCTOR-------------------------------------

client::~client()
{}