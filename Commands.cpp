#include "Commands.hpp"
#include "Server.hpp"


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
		sendReply(cl, Replies::yourHost(cl.getNick()));
		sendReply(cl, Replies::created(cl. getNick(), _serv->getCreationDate()));
		sendReply(cl, Replies::myInfo(cl.getNick()));
		sendReply(cl, Replies::isSupport(cl.getNick()));
	}
}

void Commands::messBot(Channel* channel)
{
	std::string lineBot = ":mybot!bot@localhost PRIVMSG ";
    lineBot += channel->getName();
    lineBot += " :feur :)\r\n";
	std::vector<client*> users = channel->getUserList();
	for(std::vector<client*>::iterator it = users.begin(); it < users.end(); it++)
    {
		send((*it)->getSocketFd(), lineBot.c_str(), lineBot.size(), 0);
	}
}

void Commands::broadcast(client& cl, Message& msg, Channel* channel)
{
    if(channel->user_present(cl) == true)
    {
		std::vector<client*> users = channel->getUserList();
		std::string message = Relay::privmsg(Relay::prefix(cl.getNick(), cl.getUsername()), channel->getName(), msg.params.back()) + "\r\n";
        for(std::vector<client*>::iterator it = users.begin(); it < users.end(); it++)
        {
            if(*it == &cl)
                continue ;

            send((*it)->getSocketFd(), message.c_str(), message.size(), 0);
        }
    }
    else
    {
        std::string out = ":ircserv " + Replies::notOnChannel(cl.getNick(), channel->getName()) + "\r\n";
	    send(cl.getSocketFd(), out.c_str(), out.size(), 0);
    }
}

void Commands::broadcastToChannel(std::string msg, Channel *channel)
{
	msg += "\r\n";
	std::vector<client*> users = channel->getUserList();
	for(std::vector<client*>::iterator it = users.begin(); it < users.end(); it++)
	{
		send((*it)->getSocketFd(), msg.c_str(), msg.size(), 0);
	}
}

// SERV HELPERS -------------------------------

void Commands::disconnectClient(client &cl, const std::string &reason)
{
	std::vector<Channel*> channelList = cl.getChannels();
	std::vector<Channel*>::iterator it;
	for (it = channelList.begin(); it != channelList.end(); it++)
	{
		broadcastToChannel(Relay::quit(Relay::prefix(cl.getNick(), cl.getUsername()), reason), *it);
		(*it)->removeUser(cl);
		if ((*it)->isOperator(cl))
			(*it)->removeOperator(cl);
		cl.removeChannel(*it);
	}
}

Channel* Commands::getChannel(client& cl, const std::string& rawName)
{
	if (rawName.size() < 2 || rawName[0] != '#')
	{
		sendReply(cl, Replies::badChanMask(cl.getNick(), rawName));
		return NULL;
	}
	std::map<std::string, Channel*>& channelList = _serv->getChannelList();
	std::map<std::string, Channel*>::iterator it = channelList.find(rawName);
	if (it == channelList.end())
	{
		sendReply(cl, Replies::noSuchChannel(cl.getNick(), rawName));
		return NULL;
	}
	return it->second;
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
	bool wasRegistered = cl.isRegistered();
	std::string oldPrefix = Relay::prefix(cl.getNick(), cl.getUsername());
	cl.setNick(newNick);
	if (wasRegistered)
	{
		std::string nickLine = Relay::nickChange(oldPrefix, newNick);
		std::vector<Channel*> channels = cl.getChannels();
		if (channels.empty())
			sendLine(cl, nickLine);
		else
		{
			for (std::vector<Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
				broadcastToChannel(nickLine, *it);
		}
	}
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
		broadcast(cl, msg, channel);
		if(msg.params[1] == "quoi")
		{
			messBot(channel);
		}
		return;
    }
	const std::string& target = msg.params[0];
	client* recipient = getUser(cl, target);
	if (recipient == NULL)
		return;
	sendLine(*recipient, Relay::privmsg(Relay::prefix(cl.getNick(), cl.getUsername()), target, msg.params.back()));
}

//--------QUIT--------

void Commands::removeClientFromChannels(client& cl, const std::string& reason)
{
	std::vector<Channel*> channelList = cl.getChannels();
	std::vector<Channel*>::iterator it;
	for (it = channelList.begin(); it != channelList.end(); it++) {
		if ((*it)->isOperator(cl))
			(*it)->removeOperator(cl);
		(*it)->removeUser(cl);
		cl.removeChannel(*it);
		broadcastToChannel(Relay::quit(Relay::prefix(cl.getNick(), cl.getUsername()), reason), *it);
		if ((*it)->getNumberOfUsers() == 0)
		{
			_serv->getChannelList().erase((*it)->getName());
			delete *it;
		}
		else if ((*it)->getOperatorsList().empty())
		{
			std::vector<client*>::iterator client = (*it)->getUserList().begin();
			(*it)->add_operator(**client);
		}
	}
}

void Commands::cmdQUIT(client& cl, Message msg) {
	std::string reason;
	if (msg.params.empty())
		reason = "Client Quit";
	else
		reason = msg.params[0];
	disconnectClient(cl, reason);
	removeClientFromChannels(cl, reason);
	cl.setShouldDisconnect(true);
}


//--------PING / PONG--------

void Commands::cmdPING(client& cl, Message msg)
{
	std::string token;
	if (msg.params.empty())
		token = "";
	else
		token = msg.params.back();
	sendLine(cl, ":ircserv PONG ircserv :" + token);
}

void Commands::cmdPONG(client& cl, Message msg)
{
	(void)cl;
	(void)msg;
}


//--------JOIN--------

void Commands::mess_join(client &cl, Channel *cha)
{
	std::string line = (*cha).getTopic();
	std::string topic = (*cha).getTopic();
	if(topic.empty())
		line = "331 " + cl.getRealName() + " " + (*cha).getName() + " :No topic is set";
	else
		line = "332 " + cl.getRealName() + " " + (*cha).getName() + " :" + topic;
	sendReply(cl, line);
	line = "353 " + cl.getRealName() + " = " + (*cha).getName() + " :";
	std::vector<client*> userList = (*cha).getUserList();
	for(std::vector<client*>::iterator it = userList.begin(); it < userList.end(); it++)
	{
		if((*cha).isOperator(**it))
		{
			line = line + "@";
		}
		line = line + (**it).getUsername() + " ";
	}
	line.erase(line.size() - 1);
	sendReply(cl, line);
	line = "366 "  + cl.getRealName() + " " + (*cha).getName() + " :End of NAMES list";
	sendReply(cl, line);
}


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
    std::map<std::string, Channel*>& channelList = _serv->getChannelList();
    std::map<std::string, Channel*>::iterator it;
	std::string name = msg.params[0];
    it = channelList.find(name);
    if(it != channelList.end())
    {
		if (it->second->isInviteOnly() == true) 
		{
			if(!it->second->is_invited(cl))
			{
				sendReply(cl, Replies::inviteOnlyChan(cl.getNick(), name));
				return ;
			}
		}
		if (!it->second->getChannelKey().empty())
		{
			std::string channelKey = it->second->getChannelKey();
			if (msg.params.size() < 2 || msg.params[1] != channelKey)
			{
				sendReply(cl, Replies::badChannelKey(cl.getNick(), name));
				return ;
			}
		}
		if (it->second->getNumberOfUsers() >= it->second->getUserLimit())
		{
			sendReply(cl, Replies::channelIsFull(cl.getNick(), name));
        	return ;
		}
		if(it->second->user_present(cl) == true)
        	return ;
        it->second->add_user(cl);
		cl.addChannel(it->second);
		broadcastToChannel(Relay::join(Relay::prefix(cl.getNick(), cl.getUsername()), name), it->second);
		mess_join(cl, it->second);
	}
    else
    {
        Channel *channel = new Channel(name);
        channel->add_user(cl);
        channel->add_operator(cl);
        channelList[name] = channel;
		cl.addChannel(channel);
		sendLine(cl, Relay::join(Relay::prefix(cl.getNick(), cl.getUsername()), name));
		mess_join(cl, channel);
    }
}

//--------INVITE--------

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
	channel.add_guest(*recipient);
	// recipient->addChannel(&channel);
	sendLine(*recipient, Relay::invite(Relay::prefix(cl.getNick(), cl.getUsername()), target, channel.getName()));
	sendReply(cl, Replies::inviting(cl.getNick(), channel.getName(), target));
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
	if (msg.params.size() == 1)
	{
		std::string mode = "+";
		if (channel.isInviteOnly())
			mode += "i";
		if (channel.isTopicRestricted())
			mode += "t";
		if (!channel.getChannelKey().empty())
			mode += "k";
		if (channel.isLimited())
			mode += "l";
		sendReply(cl,  Replies::channelModeIs(cl.getNick(), channel.getName(), mode));
		return ;
	}
	if (msg.params[1].size() < 2)
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
	
	if (msg.params.size() > 2)
		broadcastToChannel(Relay::mode(Relay::prefix(cl.getNick(), cl.getUsername()), channel.getName(), msg.params[1], msg.params[2]), &channel);
	else
		broadcastToChannel(Relay::mode(Relay::prefix(cl.getNick(), cl.getUsername()), channel.getName(), msg.params[1], ""), &channel);
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
		{
			channel.setUserLimit(limit);
			channel.setIsLimited(true);
		}
	}
	if (msg.params[1][0] == '-')
	{
		channel.setUserLimit(INT_MAX);
		channel.setIsLimited(false);
	}
}


//--------KICK--------

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
	{
		sendReply(cl, Replies::noSuchNick(cl.getNick(), target));
		return;
	}
	if (!channel.user_present(*recipient))
	{
		sendReply(cl, Replies::userNotInChannel(cl.getNick(), target, channel.getName()));
		return;
	}
	if (recipient == &cl)
	{
		sendReply(cl, "Error: you cannot KICK yourself");
		return;
	}
	if (channel.isOperator(*recipient))
		channel.removeOperator(*recipient);
	channel.removeUser(*recipient);
	recipient->removeChannel(&channel);
	std::string reason;
	if (msg.params.size() > 2)
		reason = msg.params.back();
	else
		reason = "";
	std::string kickLine = Relay::kick(Relay::prefix(cl.getNick(), cl.getUsername()), channel.getName(), target, reason);
	sendLine(*recipient, kickLine);
	broadcastToChannel(kickLine, &channel);
	if (channel.getNumberOfUsers() == 0)
	{
		_serv->getChannelList().erase(channel.getName());
		delete &channel;
	}
	else if (channel.getOperatorsList().empty())
	{
		std::vector<client*>::iterator client = channel.getUserList().begin();
		channel.add_operator(**client);
	}
}


//--------TOPIC--------

void Commands::cmdTOPIC(client &cl, Message msg, Channel &channel)
{
	if (!channel.user_present(cl))
	{
		sendReply(cl, Replies::notOnChannel(cl.getNick(), channel.getName())); 
		return ;
	}
	if (channel.isTopicRestricted() == true && !channel.isOperator(cl)) 
	{
		sendReply(cl, Replies::chanOpPrivsNeeded(cl.getNick(), channel.getName()));
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
		sendReply(cl, Replies::topic(cl.getNick(), channel.getName(), channel.getTopic()));
		return;
	}
	else
	{
		std::string topic = msg.params.back();
		channel.setTopic(topic);
		broadcastToChannel(Relay::topicChange(Relay::prefix(cl.getNick(), cl.getUsername()), channel.getName(), channel.getTopic()), &channel);
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
	if (msg.command == "PING")
	{
		cmdPING(cl, msg);
		return;
	}
	if (msg.command == "PONG")
	{
		cmdPONG(cl, msg);
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
		if (msg.command == "NICK")
			cmdNICK(cl, msg);
		else if (msg.command == "USER" || msg.command == "PASS")
			sendReply(cl, Replies::alreadyRegistered(selfNick(cl)));
		else if (msg.command == "PRIVMSG")
			cmdPRIVMSG(cl, msg);
		else if (msg.command == "JOIN")
			cmdJOIN(cl, msg);
		else if (msg.command == "MODE" || msg.command == "KICK" || msg.command == "INVITE" || msg.command == "TOPIC")
		{
			if (msg.params.empty())
			{
				sendReply(cl,  Replies::needMoreParams(cl.getNick(), msg.command));
				return;
			}
			size_t channelIndex;
			if (msg.command == "INVITE")
				channelIndex = 1;
			else
				channelIndex = 0;
			if (msg.params.size() <= channelIndex)
			{
				sendReply(cl,  Replies::needMoreParams(cl.getNick(),  msg.command));
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
