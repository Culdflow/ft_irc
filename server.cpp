#include "irc.hpp"

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
}

void	serv::init()
{
	int clientSocket = accept(server.getSocketFd(), NULL, NULL);

	char buffer[1024] = { 0 };
	recv(clientSocket, buffer, sizeof(buffer), 0);
	std::cout << "Message from client: " << buffer << std::endl;
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