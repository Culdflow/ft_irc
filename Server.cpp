#include "Server.hpp"
#include <cctype>

//HELPER-----------------------------------------------------------

static bool isValidNick(const std::string& nick)
{
	if (nick.empty() || nick.size() > 9)
		return false;
	if (!isalpha(nick[0]))
		return false;
	for (size_t i = 1; i < nick.size(); i++)
	{
		char c = nick[i];
		if (!isalnum(c) && c != '-' && c != '_' && c != '[' && c != ']'
			&& c != '\\' && c != '^' && c != '{' && c != '}' && c != '`')
			return false;
	}
	return true;
}

std::vector<std::string> split(const std::string& s, char delimiter) {
	std::vector<std::string> tokens;
	std::stringstream ss(s);
	std::string token;

	while (std::getline(ss, token, delimiter)) {
		tokens.push_back(token);
	}

	return tokens;
}

//CONSTRUCTORS------------------------------------------------------

serv::serv(unsigned int port, const std::string& pass) : _port(port), _password(pass)
{
	std::cout << "Serv Constructor Called" << std::endl;
	std::cout << "port = " << this->_port << std::endl;
	std::cout << "password = " << this->_password << std::endl;
	this->createSocket();
}

serv::serv()
{
	std::cout << "Serv Constructor Called" << std::endl;
	this->_port = 6667;
	std::cout << "port = " << this->_port << std::endl;
	this->_password = "123456789";
	std::cout << "password = " << this->_password << std::endl;
	this->createSocket();
}

serv::serv(const serv& src)
{
	if (this != &src)
		*this = src;
}

//OPERATORS---------------------------------------------------------

serv&	serv::operator=(const serv& src)
{
	this->_password = src._password;
	this->_port = src._port;
	this->_socket = src._socket;
	this->_socketFd = src._socketFd;
	this->_pollFds = src._pollFds;
	this->_clientList = src._clientList;
	return (*this);
}

//PRIVATE METHODS----------------------------------------------------

void serv::createSocket()
{
	this->_socketFd = socket(AF_INET, SOCK_STREAM, 0);
	this->_socket.sin_family = AF_INET;
	this->_socket.sin_port = htons(this->_port);
	this->_socket.sin_addr.s_addr = INADDR_ANY;
	bind(_socketFd, (struct sockaddr*)&_socket, sizeof(_socket));
	listen(_socketFd, 0);

	pollfd listenPfd;
	listenPfd.fd = _socketFd;
	listenPfd.events = POLLIN;
	listenPfd.revents = 0;
	_pollFds.push_back(listenPfd);
}

void serv::acceptNewClient() {
	int cl = accept(this->_socketFd, NULL, NULL);
	struct sockaddr_storage addr;

	socklen_t addr_len = sizeof(addr);

	if (getpeername(cl, (struct sockaddr*)&addr, &addr_len) == -1)
	{
		std::cerr << "errrrorrrrr getpeername" << std::endl;
		close(cl);
		return;
	}
	this->_clientList.push_back(new client(cl));

	pollfd clientPfd;
	clientPfd.fd = cl;
	clientPfd.events = POLLIN;
	clientPfd.revents = 0;
	_pollFds.push_back(clientPfd);
}
		
void serv::handleClient(int fd) {
	for (std::vector<client*>::iterator c = this->_clientList.begin(); c != this->_clientList.end(); c++)
	{
		if ((*c)->getSocketFd() == fd) {
			bool closed = false;
			std::vector<Message> msgs = socketBufferParsing(**c, closed);
			if (closed) {
				close(fd);
				removePollFd(fd);
				delete *c;
				_clientList.erase(c);
				return;
			}
			for (size_t j = 0; j < msgs.size(); j++)
				this->recvMsg(**c, msgs[j]);
			return;
		}
	}

}

void serv::removePollFd(int fd)
{
	for (std::vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); it++)
	{
		if (it->fd == fd)
		{
			_pollFds.erase(it);
			return;
		}
	}
}

void serv::sendReply(client& cl, const std::string& reply) {
	std::string line = ":ircserv " + reply + "\r\n";
	send(cl.getSocketFd(), line.c_str(), line.size(), 0);
}

void serv::sendWelcome(client& cl)
{
    std::string nick = cl.getNick();
    std::string prefix = ":ircserv ";
	std::string line001 = prefix + "001 " + nick + " :Welcome to the IRC network\r\n";

	//debug
	std::cout << "SENDING: [" << line001 << "]" << std::endl;
	//
    send(cl.getSocketFd(), line001.c_str(), line001.size(), 0);
}

void serv::checkRegistration(client& cl)
{
    if (cl.isRegistered())  
        return;

    if (cl.isNameSet() && cl.isNickSet() && cl.isPaswdCorrect())
    {
        cl.setRegistered(true);
        sendWelcome(cl);
    }
}

void serv::cmdPass(client& cl, Message msg) {
	if (cl.isPaswdCorrect())
	{
		sendReply(cl, "462 * :You may not reregister");
		return;
	}
	if (msg.params.empty())
	{
		sendReply(cl, "461 * PASS :Not enough parameters");
		return;
	}
	if (msg.params[0] != this->_password)
	{
		sendReply(cl, "464 * :Password incorrect");
		return;
	}
	cl.setPaswdCorrect(true);
	checkRegistration(cl);
}

void serv::cmdNick(client& cl, Message msg) {
	if (msg.params.empty())
	{
		sendReply(cl, "431 * :No nickname given");
		return;
	}
	const std::string& newNick = msg.params[0];
	if (!isValidNick(newNick))
	{
		sendReply(cl, "432 * " + newNick + " :Erroneous nickname");
		return;
	}
	for (std::vector<client*>::iterator it = _clientList.begin(); it != _clientList.end(); it++)
	{
		if (*it != &cl && (*it)->getNick() == newNick)
		{
			sendReply(cl, "433 * " + newNick + " :Nickname is already in use");
			return;
		}
	}
	cl.setNick(newNick);
	checkRegistration(cl);
}

void serv::cmdName(client& cl, Message msg) {
	if (msg.params.size() < 4)
	{
		sendReply(cl, "461 * USER :Not enough parameters");
		return;
	}
	if (cl.isNameSet())
	{
		sendReply(cl, "462 * :You may not reregister");
		return;
	}
	cl.setName(msg.params[0]);
	checkRegistration(cl);
}



//METHODS

void	serv::recvMsg(client& cl, Message msg)
{
	//debug
	std::cout << "prefix: " << msg.prefix << std::endl;
	std::cout << "command: " << msg.command << std::endl;
	for (std::vector<std::string>::iterator it = msg.params.begin(); it != msg.params.end(); it++)
		std::cout << "param: " << *it << std::endl;
	//
	
	if (!cl.isNameSet() || !cl.isNickSet() || !cl.isPaswdCorrect())
	{
		if (msg.command == "PASS")
			cmdPass(cl, msg);
		else if (msg.command == "NICK")
			cmdNick(cl, msg);
		else if (msg.command == "USER")
			cmdName(cl, msg);
	}
	else
	{
		//gere tous les autres msgs
	}
}
void	serv::run()
{
	while (true)
	{
		int ret = poll(&_pollFds[0], _pollFds.size(), -1);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "errrrorrrrr poll" << std::endl;
			exit(EXIT_FAILURE); //gerer les deconnexions etc
		}
		for (size_t i = 0; i < _pollFds.size(); i++)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				if (_pollFds[i].fd == _socketFd)
					acceptNewClient();
				else
					handleClient(_pollFds[i].fd);
			}
		}
	}
}

//GETTER-------------------------------------------------------------

sockaddr_in	serv::getSocket()const
{
	return (this->_socket);
}

unsigned int	serv::getPort()const
{
	return (this->_port);
}

std::string		serv::getPassword()const
{
	return (this->_password);
}

int				serv::getSocketFd()const
{
	return (this->_socketFd);
}

//EXCEPTIONS-------------------------------------------------------------


//DESTRUCTORS--------------------------------------------------------------

serv::~serv()
{
	for (size_t i = 0; i < _clientList.size(); i++) {
		close(_clientList[i]->getSocketFd());
        delete _clientList[i];
	}
	close(_socketFd);
	std::cout << "serv destructor called" << std::endl;
}