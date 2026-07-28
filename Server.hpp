#pragma once

#include "ft_irc.hpp"

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
		unsigned int			_port;
		std::string				_password;
		sockaddr_in				_socket;
		int						_socketFd;
		fd_set					_readySockets;
		fd_set					_currentSockets;
		std::vector<client*>	_clientList;
		std::vector<t_channel> 	_channelList;

		//PRIVATE METHODS
		void 			createSocket();
		void 			acceptNewClient();
		void 			handleClient(int fd);
		void 			sendReply(client& cl, const std::string& reply);
		void 			cmdPass(client& cl, Message msg);
		void 			cmdNick(client& cl, Message msg);
		void 			cmdName(client& cl, Message msg);
		void 			checkRegistration(client& cl);
		void			sendWelcome(client& cl);


	public:
		//CONSTRUCTORS
		serv();
		serv(unsigned int port, const std::string& pass);
		serv(const serv& src);
		serv& operator=(const serv& src);
		//DESTRUCTORS
		~serv();


		//METHODS
		void			recvMsg(client &cl, Message msg);
		void			run();

		//GETTER
		sockaddr_in		getSocket()const;
		unsigned int 	getPort()const;
		std::string		getPassword()const;
		int				getSocketFd()const;
};

std::vector<std::string> split(const std::string& s, char delimiter);