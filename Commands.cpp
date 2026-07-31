#include "Commands.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "Replies.hpp"
#include <cctype> 

//HELPER-----------------------------------------------------------

static bool isValidNick(const std::string& nick)
{
	if (nick.empty() || nick.size() > 9)
		return false;
	if (!isalpha(nick[0]))
		return false;
	for (size_t i = 1; i < nick.size(); i++)
	{
		char c = nick[i];
		if (!isalnum(c) && c != '-' && c != '_' && c != '[' && c != ']'
			&& c != '\\' && c != '^' && c != '{' && c != '}' && c != '`')
			return false;
	}
	return true;
}

static std::string selfNick(client& cl)
{
	if (cl.getNick().empty())
		return "*";
	return cl.getNick();
}

//CONSTRUCTORS------------------------------------------------------

Commands::Commands(serv* server) : _serv(server) {}

Commands::Commands(const Commands& src)
{
	if (this != &src)
		*this = src;
}

//OPERATORS---------------------------------------------------------

Commands& Commands::operator=(const Commands& src)
{
	this->_serv = src._serv;
	return (*this);
}

//SEND HELPERS-------------------------------------------------------

void Commands::sendLine(client& cl, const std::string& line) {
	std::string out = line + "\r\n";
	send(cl.getSocketFd(), out.c_str(), out.size(), 0);
}

void Commands::sendReply(client& cl, const std::string& reply) {
	sendLine(cl, ":ircserv " + reply);
}

void Commands::checkRegistration(client& cl)
{
	if (cl.isRegistered())
		return;

	if (cl.isUsernameSet() && cl.isNickSet() && cl.isPaswdCorrect())
	{
		cl.setRegistered(true);
		sendReply(cl, Replies::welcome(cl.getNick()));
	}
}

Channel* Commands::getChannel(client& cl, const std::string& rawName)
{
	if (rawName.size() < 2 || rawName[0] != '#')
	{
		sendReply(cl, Replies::badChanMask(cl.getNick(), rawName));
		return NULL;
	}
	std::map<std::string, Channel>& channelList = _serv->getChannelList();
	std::map<std::string, Channel>::iterator it = channelList.find(rawName);
	if (it == channelList.end())
	{
		sendReply(cl, Replies::noSuchChannel(cl.getNick(), rawName));
		return NULL;
	}
	return &it->second;
}

client* Commands::getUser(client& cl, const std::string& nick)
{
	client* recipient = _serv->findClientByNick(nick);
	if (recipient == NULL)
	{
		sendReply(cl, Replies::noSuchNick(cl.getNick(), nick));
		return NULL;
	}
	return recipient;
}

//COMMANDS-------------------------------------------------------------

//--------AUTHENTIFICATION--------

void Commands::cmdPASS(client& cl, Message msg) {
	if (cl.isPaswdCorrect())
	{
		sendReply(cl, Replies::alreadyRegistered(selfNick(cl)));
		return;
	}
	if (msg.params.empty())
	{
		sendReply(cl, Replies::needMoreParams(selfNick(cl), "PASS"));
		return;
	}
	if (msg.params[0] != _serv->getPassword())
	{
		sendReply(cl, Replies::passwdMismatch(selfNick(cl)));
		cl.setShouldDisconnect(true);
		return;
	}
	cl.setPaswdCorrect(true);
	checkRegistration(cl);
}

void Commands::cmdNICK(client& cl, Message msg) {
	if (msg.params.empty())
	{
		sendReply(cl, Replies::noNicknameGiven(selfNick(cl)));
		return;
	}
	const std::string& newNick = msg.params[0];
	if (!isValidNick(newNick))
	{
		sendReply(cl, Replies::erroneusNickname(selfNick(cl), newNick));
		return;
	}
	client* existing = _serv->findClientByNick(newNick);
	if (existing != NULL && existing != &cl)
	{
		sendReply(cl, Replies::nicknameInUse(selfNick(cl), newNick));
		return;
	}
	cl.setNick(newNick);
	checkRegistration(cl);
}

void Commands::cmdUSER(client& cl, Message msg) {
	if (msg.params.size() < 4)
	{
		sendReply(cl, Replies::needMoreParams(selfNick(cl), "USER"));
		return;
	}
	if (cl.isUsernameSet())
	{
		sendReply(cl, Replies::alreadyRegistered(selfNick(cl)));
		return;
	}
	cl.setUsername(msg.params[0]);
	cl.setRealName(msg.params.back());
	checkRegistration(cl);
}

//--------PRIVMSG--------

void Commands::cmdPRIVMSG(client& cl, Message msg) {
	if (msg.params.empty())
	{
		sendReply(cl, Replies::noRecipient(cl.getNick(), "PRIVMSG"));
		return;
	}
	if (msg.params.size() < 2)
	{
		sendReply(cl, Replies::noTextToSend(cl.getNick()));
		return;
	}
	if(msg.params[0][0] == '#')
    {
		Channel* channel = getChannel(cl, msg.params[0]);
		if (channel == NULL)
			return;
		channel->broadcast(cl, msg);
		return;
    }
	const std::string& target = msg.params[0];
	client* recipient = getUser(cl, target);
	if (recipient == NULL)
		return;
	std::string message;
	for (std::vector<std::string>::iterator it = msg.params.begin() + 1; it != msg.params.end(); it++)
		message = message + *it + " ";
	std::cout<<"yoooooooooo\n";
	sendLine(*recipient, Relay::privmsg(Relay::prefix(cl.getNick(), cl.getUsername()), target, message));
}

//--------QUIT--------

void Commands::cmdQUIT(client& cl, Message msg) {
	if (msg.params.empty())
		sendLine(cl, "ERROR :Closing Link: Client Quit");
	else
		sendLine(cl, "ERROR :Closing Link: " + msg.params[0]);
	cl.setShouldDisconnect(true);
	//TO DO --- 
	// Broadcast dans les channel ou il était présent
	// Relay::quit(Relay::prefix(cl.getNick(), cl.getUsername()), reason)
}

//--------JOIN--------

void Commands::cmdJOIN(client& cl, Message msg)
{
    if(msg.params.size() < 1)
    {
        sendReply(cl, Replies::needMoreParams(cl.getNick(), "JOIN"));
        return ;
    }
    if(msg.params[0][0] != '#' || (msg.params[0][0] && msg.params[0][1] == '\0'))
    {
        sendReply(cl, Replies::badChanMask(cl.getNick(), msg.params[0]));
        return ;
    }
    std::map<std::string, Channel>& channelList = _serv->getChannelList();
    std::map<std::string, Channel>::iterator it;
	std::string name = msg.params[0];
    it = channelList.find(name);
    if(it != channelList.end())
    {
		if (it->second.isInviteOnly() == true) 
		{
			sendReply(cl, Replies::inviteOnlyChan(cl.getNick(), name));
        	return ;
		}
		if (!it->second.getChannelKey().empty())
		{
			std::string channelKey = it->second.getChannelKey();
			if (msg.params.size() < 2 || msg.params[1] != channelKey)
			{
				sendReply(cl, Replies::badChannelKey(cl.getNick(), name));
				return ;
			}
		}
		if (it->second.getNumberOfUsers() >= it->second.getUserLimit())
		{
			sendReply(cl, Replies::channelIsFull(cl.getNick(), name));
        	return ;
		}
        it->second.add_user(cl);
    }
    else
    {
		std::cout << "salut la team\n"; // verifier que y a bien un channel apres #
        Channel channel(name);
        channel.add_user(cl);
        channel.add_operator(cl);
        channelList[name] = channel;
    }
}

//--------MODE--------

void 	Commands::cmdMODE(client& cl, Message msg, Channel& channel) {
	if (!channel.user_present(cl))
	{
		sendReply(cl, Replies::notOnChannel(cl.getNick(), channel.getName()));       
		return ;
	}
	if (!channel.isOperator(cl))
	{
		sendReply(cl, Replies::chanOpPrivsNeeded(cl.getNick(), channel.getName()));
        return ;
	}
	if (msg.params.size () < 2  || msg.params[1].size() < 2)
	{
		sendReply(cl,  Replies::needMoreParams(cl.getNick(), "MODE"));
		return ;
	}
	char c = msg.params[1][1];
	if (msg.params[1].size() > 2 || (c != 'i' && c != 't' && c != 'k' && c != 'o' && c != 'l'))
	{
		sendReply(cl, Replies::unknownMode(cl.getNick(), std::string(1, c)));
		return ;
	}
	if (c == 'i')
		cmdIMODE(cl, msg, channel);
	else if (c == 't')
		cmdTMODE(cl, msg, channel);
	else if (c == 'k')
		cmdKMODE(cl, msg, channel);
	else if (c == 'o')
		cmdOMODE(cl, msg, channel);
	else if (c == 'l')
		cmdLMODE(cl, msg, channel);
	
	//TO DO -----
	//broadcast au channel le mode utilisé
	// if (msg.params.size() > 2)
	// 	broadcast(Relay::mode(Relay::prefix(cl.getNick(), cl.getUsername()), channel.getName(), msg.params[1], msg.params[2]));
	// else
	// 	broadcast(Relay::mode(Relay::prefix(cl.getNick(), cl.getUsername()), channel.getName(), msg.params[1], ""));

}

void 	Commands::cmdIMODE(client&cl, Message msg, Channel &channel) {
	(void)cl;
	if (msg.params[1][0] == '+')
		channel.setInviteOnly(true);
	else if (msg.params[1][0] == '-')
		channel.setInviteOnly(false);
}

void 	Commands::cmdTMODE(client&cl, Message msg, Channel &channel) {
	(void)cl;
	if (msg.params[1][0] == '+')
		channel.setTopicRestricted(true);
	else if (msg.params[1][0] == '-')
		channel.setTopicRestricted(false);
}

void 	Commands::cmdKMODE(client&cl, Message msg, Channel &channel) {
	std::string key = "";
	if (msg.params[1][0] == '+') {
		if (msg.params.size() < 3)
		{
			sendReply(cl, Replies::needMoreParams(cl.getNick(), "MODE"));
			return ;
		}
		channel.setChannelKey(msg.params[2]); //key a verifier ? 
	}
	else if (msg.params[1][0] == '-')
		channel.setChannelKey(key);
}

void 	Commands::cmdOMODE(client&cl, Message msg, Channel &channel) {
	if (msg.params.size() < 3)
	{
        sendReply(cl,  Replies::needMoreParams(cl.getNick(), "MODE"));
        return ;
    }
	const std::string& target = msg.params[2];
	client* recipient = getUser(cl, target);
	if (recipient == NULL)
		return;
	if (!channel.user_present(*recipient))
	{
		sendReply(cl, Replies::userNotInChannel(cl.getNick(), target, channel.getName()));
		return;
	}
	if (msg.params[1][0] == '+')
	{
		if (!channel.isOperator(*recipient))
			channel.add_operator(*recipient);
	}
	else if (msg.params[1][0] == '-')
	{
		if (channel.isOperator(*recipient))
			channel.removeOperator(*recipient);
	}
}
	
void 	Commands::cmdLMODE(client&cl, Message msg, Channel &channel) {
	if (msg.params[1][0] == '+') {
		if (msg.params.size() < 3)
		{
        	sendReply(cl,  Replies::needMoreParams(cl.getNick(), "MODE"));
        	return ;
    	}
		long limit;
		char *end;
		limit = strtol(msg.params[2].c_str(), &end, 10);
		if (*end != '\0' || limit <= 0 || limit > INT_MAX) 
		{
        	sendReply(cl,  Replies::needMoreParams(cl.getNick(), "MODE"));
        	return ;
    	}
		else
			channel.setUserLimit(limit);
	}
	if (msg.params[1][0] == '-')
		channel.setUserLimit(INT_MAX);
}


//--------KICK--------
// msg.command = KICK
// msg.params[0] = channel checke par dispatch
// msg.params[1] = user
// msg.params[2] = :reason
void Commands::cmdKICK(client &cl, Message msg, Channel& channel)
{
	if (!channel.user_present(cl))
	{
		sendReply(cl, Replies::notOnChannel(cl.getNick(), channel.getName()));
        return ;
	}
	if (!channel.isOperator(cl))
	{
		sendReply(cl, Replies::chanOpPrivsNeeded(cl.getNick(), channel.getName()));
        return ;
	}
	if (msg.params.size() < 2)
	{
		sendReply(cl,  Replies::needMoreParams(cl.getNick(), "KICK"));
		return;
	}
	const std::string& target = msg.params[1];
	client* recipient = getUser(cl, target);
	if (recipient == NULL)
		return;
	if (!channel.user_present(*recipient))
	{
		sendReply(cl, Replies::userNotInChannel(cl.getNick(), target, channel.getName()));
		return;
	}
	if (channel.isOperator(*recipient))
		channel.removeOperator(*recipient);
	channel.removeUser(*recipient);

	// pour envoyer le message a tous les users du channel si commentaire
	if (msg.params.size() > 2) {
		std::string comment;
		for (size_t i = 2; i < msg.params.size(); i++) {
			comment = comment + " " + msg.params[i];
		}
	}
}


//--------INVITE--------
// msg.command = INVITE
// msg.params[0] = user 
// msg.params[1] = channel checke par dispatch
void Commands::cmdINVITE(client &cl, Message msg, Channel &channel)
{
	if (!channel.user_present(cl))
	{
		sendReply(cl, Replies::notOnChannel(cl.getNick(), channel.getName()));        
		return ;
	}
	if (!channel.isOperator(cl))
	{
		sendReply(cl, Replies::chanOpPrivsNeeded(cl.getNick(), channel.getName()));
        return ;
	}
	if (msg.params.size() < 2)
	{
		sendReply(cl,  Replies::needMoreParams(cl.getNick(), "INVITE"));
		return;
	}
	const std::string& target = msg.params[0];
	client* recipient = getUser(cl, target);
	if (recipient == NULL)
		return;
	if (channel.user_present(*recipient))
	{
		sendReply(cl, Replies::userOnChannel(cl.getNick(), target, channel.getName()));
		return;
	}
	channel.add_user(*recipient);
	sendLine(*recipient, Relay::invite(Relay::prefix(cl.getNick(), cl.getUsername()), target, channel.getName()));
	sendReply(cl, Replies::inviting(cl.getNick(), channel.getName(), target));
}

//--------TOPIC--------
// msg.command = TOPIC
// msg.params[0] = channel checke par dispatch
// msg.params[1] = optionnel, changement de topic ou suppression 

//   TOPIC #test :New topic          ; Setting the topic on "#test" to "New topic".

//   TOPIC #test :                   ; Clearing the topic on "#test"

//   TOPIC #test                     ; Checking the topic for "#test"


void Commands::cmdTOPIC(client &cl, Message msg, Channel &channel)
{
	if (!channel.user_present(cl))
	{
		sendReply(cl, Replies::notOnChannel(cl.getNick(), channel.getName())); 
		return ;
	}
	if (msg.params.size() == 1)
	{
		{
			if (channel.getTopic().empty())
			{
				sendReply(cl, Replies::notopic(cl.getNick(), channel.getName()));
				return;
			}
		}
		sendLine(cl, Replies::topic(cl.getNick(), channel.getName(), channel.getTopic()));
		return;
	}
	else
	{
		if (channel.isTopicRestricted() == true && !channel.isOperator(cl)) 
		{
			sendReply(cl, Replies::chanOpPrivsNeeded(cl.getNick(), channel.getName()));
			return ;
		}
		if (msg.params[1][0] == ':' && msg.params[1].size() == 1)
		{
			std::string topicErase = "";
			channel.setTopic(topicErase);
		}
		else
		{
			channel.setTopic(msg.params[1]);
		}
		// TO DO -----
		//broadcast(Relay::topicChange(Relay::prefix(cl.getNick(), cl.getUsername()), channel.getName(), channel.getTopic()));
	}
	 

}

//DISPATCH---------------------------------------------------------

void Commands::dispatch(client& cl, Message msg)
{
	//debug
	std::cout << "prefix: " << msg.prefix << std::endl;
	std::cout << "command: " << msg.command << std::endl;
	for (std::vector<std::string>::iterator it = msg.params.begin(); it != msg.params.end(); it++)
		std::cout << "param: " << *it << std::endl;
	//

	if (msg.command == "QUIT")
	{
		cmdQUIT(cl, msg);
		return;
	}
	if (!cl.isUsernameSet() || !cl.isNickSet() || !cl.isPaswdCorrect())
	{
		if (msg.command == "PASS")
			cmdPASS(cl, msg);
		else if (msg.command == "NICK")
			cmdNICK(cl, msg);
		else if (msg.command == "USER")
			cmdUSER(cl, msg);
		else if (cmd_exist(msg.command) == true)
		{
			sendReply(cl, Replies::notRegistered(selfNick(cl)));
			return;
		}
	}
	else if (cmd_exist(msg.command) == true)
	{
		if (msg.command == "PRIVMSG")
			cmdPRIVMSG(cl, msg);
		else if (msg.command == "JOIN")
		{
			cmdJOIN(cl, msg);
			std::cout << "parfait" << std::endl;
		}
		else if (msg.command == "MODE" || msg.command == "KICK" || msg.command == "INVITE" || msg.command == "TOPIC")
		{
			if (msg.params.empty())
			{
				sendReply(cl,  Replies::needMoreParams(cl.getNick(), "INVITE"));
				return;
			}
			size_t channelIndex;
			if (msg.command == "INVITE")
				channelIndex = 1;
			else
				channelIndex = 0;
			if (msg.params.size() <= channelIndex)
			{
				sendReply(cl,  Replies::needMoreParams(cl.getNick(), "INVITE"));
				return;
			}
			Channel* channel = getChannel(cl, msg.params[channelIndex]);
			if (!channel)
				return;
			if (msg.command == "MODE")
				cmdMODE(cl, msg, *channel);
			else if (msg.command == "INVITE")
				cmdINVITE(cl, msg, *channel);
			else if (msg.command == "KICK")
				cmdKICK(cl, msg, *channel);
			else if (msg.command == "TOPIC")
				cmdTOPIC(cl, msg, *channel);
		}
	}
	else
	{
		sendReply(cl, Replies::unknownCommand(cl.getNick(), msg.command));
		return;
	}
}

//DESTRUCTOR---------------------------------------------------------

Commands::~Commands() {}
