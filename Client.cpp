#include "Client.hpp"

//CONSTRUCTOR-------------------------------------

client::client()
{
	_socketFd = 0;
	_username = "";
	_nick = "";
	_realName = "";
	_paswdCorrect = false;
	_registered = false;
	_shouldDisconnect = false;
}

client::client(int fd)
{
	_socketFd = fd;
	_username = "";
	_nick = "";
	_realName = "";
	_paswdCorrect = false;
	_registered = false;
	_shouldDisconnect = false;
}

client::client(const client& src)
{
	if (this != &src)
		*this = src;
}

//OPERATORS-----------------------------------

client&	client::operator=(const client& src)
{
	this->_username = src._username;
	this->_nick = src._nick;
	this->_realName = src._realName;
	this->_socketFd = src._socketFd;
	this->_paswdCorrect = src._paswdCorrect;
	this->_inputBuf = src._inputBuf;
	this->_registered = src._registered;
	this->_shouldDisconnect = src._shouldDisconnect;
	return (*this);
}

//IS??----------------------------------------

bool	client::isUsernameSet()const
{
	return (!this->_username.empty());
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

bool	client::shouldDisconnect() const {
	return(this->_shouldDisconnect);
}

void 	client::removeChannel(Channel* chan)
{
    std::vector<Channel*>::iterator it = std::find(_channelList.begin(), _channelList.end(), chan);
    if (it != _channelList.end())
        _channelList.erase(it);
}

void 	client::addChannel(Channel* chan)
{
    _channelList.push_back(chan);
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
const std::string& client::getUsername() const {
	return _username;
}
const std::string& client::getRealName() const {
	return _realName;
}

std::vector<Channel *> client::getChannels() const
{
    return _channelList;
}


//SETTER----------------------------------------

void	client::setNick(std::string newNick)
{
	this->_nick = newNick;
}

void	client::setUsername(std::string newUsername)
{
	this->_username = newUsername;
}

void	client::setRealName(std::string newRealName)
{
	this->_realName = newRealName;
}

void	client::setPaswdCorrect(bool paswdCorrect)
{
	this->_paswdCorrect = paswdCorrect;
}

void	client::setRegistered(bool registered) {
	this->_registered = registered;
}

void	client::setShouldDisconnect(bool value) {
	this->_shouldDisconnect = value;
}

//DESTRUCTOR-------------------------------------

client::~client()
{}
