#ifndef SERVER_HPP
#define SERVER_HPP

#include "ft_irc.hpp"
#include "Channels.hpp"
#include "Commands.hpp"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <poll.h>
#include <cerrno>
#include <ctime>

class Channel;

class serv
{
	private:
		unsigned int			_port;
		std::string				_password;
		sockaddr_in				_socket;
		int						_socketFd;
		std::vector<pollfd>		_pollFds;
		std::vector<client*>	_clientList;
		std::map<std::string, Channel*> 	_channelList;
		Commands				_commands;
		std::string				_creationDate;

		//PRIVATE METHODS
		void 			createSocket();
		void 			acceptNewClient();
		void 			handleClient(int fd);
		void 			removePollFd(int fd);

	public:
		//CONSTRUCTORS
		serv();
		serv(unsigned int port, const std::string& pass);
		serv(const serv& src);
		serv& operator=(const serv& src);
		//DESTRUCTORS
		~serv();


		//METHODS
		void			run();
		client*			findClientByNick(const std::string& nick);
		bool			channel_exist(std::string& name_ch);
		
		//GETTER
		sockaddr_in		getSocket()const;
		unsigned int 	getPort()const;
		std::string		getPassword()const;
		int				getSocketFd()const;
		std::map<std::string, Channel*>& getChannelList();
		const std::string& 	getCreationDate() const;

};

std::vector<std::string> split(const std::string& s, char delimiter);

#endif