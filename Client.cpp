#include "Client.hpp"

//CONSTRUCTOR-------------------------------------

client::client()
{
	_socketFd = 0;
	_name = "";
	_nick = "";
	_paswdCorrect = false;
	_registered = false;
}

client::client(int fd)
{
	_socketFd = fd;
	_name = "";
	_nick = "";
	_paswdCorrect = false;
	_registered = false;
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
	this->_registered = src._registered;
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

bool	client::isRegistered() const {
	return(this->_registered);
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

void	client::setRegistered(bool registered) {
	this->_registered = registered;
}

//DESTRUCTOR-------------------------------------

client::~client()
{}