#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "ft_irc.hpp"
#include "utils.hpp"
#include "Replies.hpp"
#include <cctype> 
#include <climits>

class serv;
class Channel;

class Commands
{
	private:
		serv*	_serv;

		void		sendLine(client& cl, const std::string& line);
		void		sendReply(client& cl, const std::string& reply);
		void 		broadcast(client& cl, Message& msg, Channel* channel);
		void		broadcastToChannel(std::string msg, Channel* channel);
		void		checkRegistration(client& cl);
		Channel*	getChannel(client& cl, const std::string& rawName);
		client*		getUser(client& cl, const std::string& nick);

	public:
		Commands(serv* server);
		Commands(const Commands& src);
		Commands&	operator=(const Commands& src);
		~Commands();

		void	dispatch(client& cl, Message msg);
		void	removeClientFromChannels(client& cl, const std::string& reason);

		void	cmdPASS(client& cl, Message msg);
		void	cmdNICK(client& cl, Message msg);
		void	cmdUSER(client& cl, Message msg);
		void 	cmdJOIN(client& cl, Message msg);
		void	cmdPRIVMSG(client& cl, Message msg);
		void	cmdQUIT(client& cl, Message msg);
		void 	cmdMODE(client& cl, Message msg, Channel& channel);
		void 	cmdIMODE(client& cl, Message msg, Channel &channel);
		void 	cmdTMODE(client& cl, Message msg, Channel &channel);		
		void 	cmdKMODE(client& cl, Message msg, Channel &channel);
		void 	cmdOMODE(client& cl, Message msg, Channel &channel);
		void 	cmdLMODE(client& cl, Message msg, Channel &channel);
		void 	cmdKICK(client& cl, Message msg, Channel& channel);
		void 	cmdINVITE(client& cl, Message msg, Channel& channel);
		void 	cmdTOPIC(client& cl, Message msg, Channel& channel);
		void	disconnectClient(client& cl, const std::string& reason);
		void	cmdPING(client& cl, Message msg);
		void	cmdPONG(client& cl, Message msg);
};

#endif