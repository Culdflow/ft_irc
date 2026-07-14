#include "server.hpp"

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

serv::~serv()
{
	std::cout << "serv destructor called" << std::endl;
}

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
					int client = accept(this->_socketFd, NULL, NULL);
					struct sockaddr_storage addr;

					socklen_t addr_len = sizeof(addr);

					if (getpeername(client, (struct sockaddr*)&addr, &addr_len) == -1)
					{
						std::cerr << "errrrorrrrr getpeername" << std::endl;
						close(client);
						return;
					}
					send(client, "new client\n", 11, 0);
					FD_SET(client, &this->_currentSockets);
				}
				else
				{
					send(i, "not new client\n", 15, 0);
					char buffer[1024];
					recv(i, &buffer, sizeof(buffer), 0);
					std::cout << buffer << std::endl;
				}
			}
		}
	}
}

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

const char* serv::PortWrongError::what(void) const throw()
{
	return ("[Error] : PORT NUMBER WRONG (must be a number between 1 and 65535)");
}