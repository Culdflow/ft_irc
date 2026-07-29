#pragma once

#include <string>

class	client
{
	private:
		std::string			_nick;
		std::string			_username;
		std::string			_realName;
		bool				_paswdCorrect;
		int					_socketFd;
		std::string			_inputBuf;
		bool				_registered;
		bool				_shouldDisconnect;

	public:
		client();
		client(int fd);
		client(const client& src);
		client&	operator=(const client& src);
		~client();

		bool	isNickSet()const;
		bool	isUsernameSet()const;
		bool	isPaswdCorrect()const;
		bool	isRegistered() const;
		bool	shouldDisconnect() const;
		//GETTER
		int					getSocketFd()const;
		const std::string& 	getNick() const;
		const std::string& 	getUsername() const;
		const std::string& 	getRealName() const;
		std::string& 		getInputBuf();
		//SETTER
		void	setNick(std::string newNick);
		void	setUsername(std::string newUsername);
		void	setRealName(std::string newRealName);
		void	setPaswdCorrect(bool paswdCorrect);
		void	setRegistered(bool registered);
		void	setShouldDisconnect(bool value);
};
