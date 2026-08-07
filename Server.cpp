#include "Server.hpp"
#include <csignal>

//HELPER-----------------------------------------------------------

static volatile sig_atomic_t g_running = 1;

static void handleSignal(int sig)
{
	(void)sig;
	g_running = 0;
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

serv::serv(unsigned int port, const std::string& pass) : _port(port), _password(pass), _commands(this)
{
	std::cout << "Serv Constructor Called" << std::endl;
	std::cout << "port = " << this->_port << std::endl;
	std::cout << "password = " << this->_password << std::endl;
	this->createSocket();
}

serv::serv() : _commands(this)
{
	std::cout << "Serv Constructor Called" << std::endl;
	this->_port = 6667;
	std::cout << "port = " << this->_port << std::endl;
	this->_password = "123456789";
	std::cout << "password = " << this->_password << std::endl;
	this->createSocket();
}

serv::serv(const serv& src) : _commands(this)
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
	signal(SIGINT, handleSignal);
	signal(SIGTERM, handleSignal);

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

	pollfd stdinPfd;
	stdinPfd.fd = STDIN_FILENO;
	stdinPfd.events = POLLIN;
	stdinPfd.revents = 0;
	_pollFds.push_back(stdinPfd);
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
				_commands.removeClientFromChannels(**c, "Connection closed");
				close(fd);
				removePollFd(fd);
				delete *c;
				_clientList.erase(c);
				return;
			}
			for (size_t j = 0; j < msgs.size(); j++)
			{
				this->_commands.dispatch(**c, msgs[j]);
				if ((*c)->shouldDisconnect())
					break;
			}
			if ((*c)->shouldDisconnect())
			{
				close(fd);
				removePollFd(fd);
				delete *c;
				_clientList.erase(c);
			}
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

//METHODS

client* serv::findClientByNick(const std::string& nick)
{
	for (std::vector<client*>::iterator it = _clientList.begin(); it != _clientList.end(); it++)
	{
		if ((*it)->getNick() == nick)
			return *it;
	}
	return NULL;
}

bool	serv::channel_exist(std::string& name_ch)
{
	std::map<std::string, Channel*>::iterator it = _channelList.find(name_ch);
	if(it == _channelList.end())
		return false;
	return true;
}

std::map<std::string, Channel*>& serv::getChannelList()
{
	return _channelList;
}

void	serv::run()
{
	bool running = true;
	while (running && g_running)
	{
		int ret = poll(&_pollFds[0], _pollFds.size(), -1);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue;
			std::cerr << "errrrorrrrr poll" << std::endl;
			return; //gerer les deconnexions etc
		}
		for (size_t i = 0; i < _pollFds.size(); i++)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				if (_pollFds[i].fd == _socketFd)
					acceptNewClient();
				else if (_pollFds[i].fd == STDIN_FILENO)
				{
					char buf[512];
					int n = read(STDIN_FILENO, buf, sizeof(buf) - 1);
					if (n <= 0)
					{
						running = false;
						break;
					}
					buf[n] = '\0';
					std::string line(buf);
					while (!line.empty() && (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r'))
						line.erase(line.size() - 1);
					if (line == "quit")
					{
						running = false;
						break;
					}
				}
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
	for (std::map<std::string, Channel*>::iterator it = _channelList.begin(); it != _channelList.end(); it++)
		delete it->second;
	close(_socketFd);
	std::cout << "serv destructor called" << std::endl;
}
