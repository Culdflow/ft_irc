#pragma once

#include <string>

class	client
{
	private:
		std::string	_nick;
		std::string	_name;
		int			_socketFd;
	
	public:
		client(int fd);
		client(const client& src);
		client&	operator=(const client& src);
		~client();

		bool	isNickSet()const;
		bool	isNameSet()const;
};