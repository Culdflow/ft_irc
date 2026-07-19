#include "server.hpp"

//HELPER-----------------------------------------------------------

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

serv::serv(char *port, char *pass)
{
	std::cout << "Serv Constructor Called" << std::endl;
	this->_port = atoi(port);
	if (this->_port <= 0 || this->_port > 65535)
		throw serv::PortWrongError();
	std::cout << "port = " << this->_port << std::endl;
	this->_password = pass;
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
	this->_readySockets = src._readySockets;
	this->_currentSockets = src._currentSockets;
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
	bind(this->_socketFd, (struct sockaddr*)&this->_socket, sizeof(this->_socket));
	listen(this->_socketFd, 0);
	FD_ZERO(&this->_currentSockets);
	FD_SET(this->_socketFd, &this->_currentSockets);
	this->init();
}

void	serv::init()
{
	while (true)
	{
		this->_readySockets = this->_currentSockets;
		if (select(FD_SETSIZE, &this->_readySockets, NULL, NULL, NULL) < 0)
		{
			std::cerr << "errrrorrrrr select" << std::endl;
			exit(EXIT_FAILURE);
		}
		for (int i = 0; i < FD_SETSIZE; i++)
		{
			if (FD_ISSET(i, &this->_readySockets))
			{
				if (i == this->_socketFd)
				{
					int cl = accept(this->_socketFd, NULL, NULL);
					struct sockaddr_storage addr;

					socklen_t addr_len = sizeof(addr);

					if (getpeername(cl, (struct sockaddr*)&addr, &addr_len) == -1)
					{
						std::cerr << "errrrorrrrr getpeername" << std::endl;
						close(cl);
						return;
					}
					client clie(cl);
					this->_clientList.push_back(clie);
					FD_SET(cl, &this->_currentSockets);
				}
				else
				{
					client cl;
					for (std::vector<client>::iterator c = this->_clientList.begin(); c < this->_clientList.end(); i++)
					{
						if ((*c).getSocketFd() == i)
							cl = *c;
					}
					char buffer[1024];
					recv(i, &buffer, sizeof(buffer), 0);
					std::string bufferStr = buffer;
					Message msg = parseCommand(bufferStr);
				}
			}
		}
	}
}

//METHODS

void	serv::recvMsg(client cl, Message msg)
{
	if (!cl.isNameSet() || !cl.isNickSet() || !cl.isPaswdSent())
	{
		//gere la creation du client avec les msgs de base
	}
	else
	{
		//gere tous les autres msgs
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

const char* serv::PortWrongError::what(void) const throw()
{
	return ("[Error] : PORT NUMBER WRONG (must be a number between 1 and 65535)");
}

//DESTRUCTORS--------------------------------------------------------------

serv::~serv()
{
	std::cout << "serv destructor called" << std::endl;
}