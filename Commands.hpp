#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "ft_irc.hpp"
#include <climits>

class serv;
class Channel;

class Commands
{
	private:
		serv*	_serv;

		void	sendLine(client& cl, const std::string& line);
		void	sendReply(client& cl, const std::string& reply);
		void	sendWelcome(client& cl);
		void	checkRegistration(client& cl);

	public:
		Commands(serv* server);
		Commands(const Commands& src);
		Commands&	operator=(const Commands& src);
		~Commands();

		void	dispatch(client& cl, Message msg);

		void	cmdPass(client& cl, Message msg);
		void	cmdNick(client& cl, Message msg);
		void	cmdUser(client& cl, Message msg);
		void 	cmdJOIN(client& cl, Message msg);
		void	cmdPrivmsg(client& cl, Message msg);
		void	cmdQuit(client& cl, Message msg);
		void 	cmdMode(client& cl, Message msg);
		void 	cmdIMode(client&cl, Message msg, Channel &channel);
		void 	cmdTMode(client&cl, Message msg, Channel &channel);		
		void 	cmdKMode(client&cl, Message msg, Channel &channel);
		void 	cmdOMode(client&cl, Message msg, Channel &channel);
		void 	cmdLMode(client&cl, Message msg, Channel &channel);
};

#endif