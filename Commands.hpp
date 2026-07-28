#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "ft_irc.hpp"

class serv;

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
		void	cmdPrivmsg(client& cl, Message msg);
		void	cmdQuit(client& cl, Message msg);
};

#endif