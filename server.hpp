#pragma once

#include "client.hpp"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>


class serv
{
	private:
		unsigned int		_port;
		std::string			_password;
		sockaddr_in			_socket;
		int					_socketFd;
		fd_set				_readySockets;
		fd_set				_currentSockets;
		std::vector<client>	_clientList;

		void		createSocket();
		void		init();
	public:
		//CONSTRUCTORS
		serv();
		serv(char *port, char *pass);
		serv(const serv& src);
		serv&	operator=(const serv& src);
		//DESTRUCTORS
		~serv();

		//getter
		sockaddr_in		getSocket()const;
		unsigned int 	getPort()const;
		std::string		getPassword()const;
		int				getSocketFd()const;

	class PortWrongError: public std::exception
	{
		public:
			virtual const char *what() const throw();
	};
};

std::vector<std::string> split(const std::string& s, char delimiter);