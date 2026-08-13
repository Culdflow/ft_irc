#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include <algorithm>

class Channel;

class	client
{
	private:
		std::string				_nick;
		std::string				_username;
		std::string				_realName;
		bool					_paswdCorrect;
		int						_socketFd;
		std::string				_inputBuf;
		bool					_registered;
		bool					_shouldDisconnect;
		std::vector<Channel*> 	_channelList;

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
		void 	removeChannel(Channel* chan);
		void 	addChannel(Channel* chan);

		//GETTER
		int					getSocketFd()const;
		const std::string& 	getNick() const;
		const std::string& 	getUsername() const;
		const std::string& 	getRealName() const;
		std::string& 		getInputBuf();
		std::vector<Channel *>& getChannels();
		//SETTER
		void	setNick(std::string newNick);
		void	setUsername(std::string newUsername);
		void	setRealName(std::string newRealName);
		void	setPaswdCorrect(bool paswdCorrect);
		void	setRegistered(bool registered);
		void	setShouldDisconnect(bool value);
};

#endif