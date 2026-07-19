#pragma once

#include <string>

class	client
{
	private:
		std::string	_nick;
		std::string	_name;
		bool		_paswdSent;
		int			_socketFd;
	
	public:
		client();
		client(int fd);
		client(const client& src);
		client&	operator=(const client& src);
		~client();

		bool	isNickSet()const;
		bool	isNameSet()const;
		bool	isPaswdSent()const;
		//GETTER
		int		getSocketFd()const;
		//SETTER
		void	setNick(std::string newNick);
		void	setName(std::string newName);
		void	setPaswdSent(bool paswdSent);
};