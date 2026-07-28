#pragma once

#include <string>

class	client
{
	private:
		std::string			_nick;
		std::string			_name;
		bool				_paswdCorrect;
		int					_socketFd;
		std::string			_inputBuf;
		bool				_registered;
	
	public:
		client();
		client(int fd);
		client(const client& src);
		client&	operator=(const client& src);
		~client();

		bool	isNickSet()const;
		bool	isNameSet()const;
		bool	isPaswdCorrect()const;
		bool	isRegistered() const;
		//GETTER
		int					getSocketFd()const;
		const std::string& 	getNick() const;
		const std::string& 	getName() const;
		std::string& 		getInputBuf();
		//SETTER
		void	setNick(std::string newNick);
		void	setName(std::string newName);
		void	setPaswdCorrect(bool paswdCorrect);
		void	setRegistered(bool registered);
};