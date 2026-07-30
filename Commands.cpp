#include "Commands.hpp"
#include "Server.hpp"
#include "utils.hpp"
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

void Commands::sendWelcome(client& cl)
{
	std::string nick = cl.getNick();
	std::string line001 = ":ircserv 001 " + nick + " :Welcome to the IRC network\r\n";
	send(cl.getSocketFd(), line001.c_str(), line001.size(), 0);
}

void Commands::checkRegistration(client& cl)
{
	if (cl.isRegistered())
		return;

	if (cl.isUsernameSet() && cl.isNickSet() && cl.isPaswdCorrect())
	{
		cl.setRegistered(true);
		sendWelcome(cl);
	}
}

Channel* Commands::getChannel(client& cl, const std::string& rawName)
{
	if (rawName.size() < 2 || rawName[0] != '#')
	{
		sendReply(cl, "476 ERR_BADCHANMASK");
		return NULL;
	}
	std::map<std::string, Channel>& channelList = _serv->getChannelList();
	std::map<std::string, Channel>::iterator it = channelList.find(rawName.substr(1));
	if (it == channelList.end())
	{
		sendReply(cl, "403 ERR_NOSUCHCHANNEL");
		return NULL;
	}
	return &it->second;
}

//COMMANDS-------------------------------------------------------------

//--------AUTHENTIFICATION--------

void Commands::cmdPASS(client& cl, Message msg) {
	if (cl.isPaswdCorrect())
	{
		sendReply(cl, "462 * :You may not reregister");
		return;
	}
	if (msg.params.empty())
	{
		sendReply(cl, "461 * PASS :Not enough parameters");
		return;
	}
	if (msg.params[0] != _serv->getPassword())
	{
		sendReply(cl, "464 * :Password incorrect");
		cl.setShouldDisconnect(true);
		return;
	}
	cl.setPaswdCorrect(true);
	checkRegistration(cl);
}

void Commands::cmdNICK(client& cl, Message msg) {
	if (msg.params.empty())
	{
		sendReply(cl, "431 * :No nickname given");
		return;
	}
	const std::string& newNick = msg.params[0];
	if (!isValidNick(newNick))
	{
		sendReply(cl, "432 * " + newNick + " :Erroneous nickname");
		return;
	}
	client* existing = _serv->findClientByNick(newNick);
	if (existing != NULL && existing != &cl)
	{
		sendReply(cl, "433 * " + newNick + " :Nickname is already in use");
		return;
	}
	cl.setNick(newNick);
	checkRegistration(cl);
}

void Commands::cmdUSER(client& cl, Message msg) {
	if (msg.params.size() < 4)
	{
		sendReply(cl, "461 * USER :Not enough parameters");
		return;
	}
	if (cl.isUsernameSet())
	{
		sendReply(cl, "462 * :You may not reregister");
		return;
	}
	cl.setUsername(msg.params[0]);
	cl.setRealName(msg.params[3]);
	checkRegistration(cl);
}

//--------PRIVMSG--------

void Commands::cmdPRIVMSG(client& cl, Message msg) {
	if (msg.params.empty())
	{
		sendReply(cl, "411 * :No recipient given (PRIVMSG)");
		return;
	}
	if (msg.params.size() < 2)
	{
		sendReply(cl, "412 * :No text to send");
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
	client* recipient = _serv->findClientByNick(target);
	if (recipient == NULL)
	{
		sendReply(cl, "401 " + target + " :No such nick/channel");
		return;
	}
	std::string message;
	for (std::vector<std::string>::iterator it = msg.params.begin() + 1; it != msg.params.end(); it++)
		message = message + *it + " ";
	std::cout<<"yoooooooooo\n";
	sendLine(*recipient, ":" + cl.getNick() + "!" + cl.getUsername() + "@ircserv PRIVMSG " + target + " :" + message);
}

//--------QUIT--------

void Commands::cmdQUIT(client& cl, Message msg) {
	if (msg.params.empty())
		sendLine(cl, "ERROR :Closing Link: Client Quit");
	else
		sendLine(cl, "ERROR :Closing Link: " + msg.params[0]);
	cl.setShouldDisconnect(true);
}

//--------JOIN--------

void Commands::cmdJOIN(client& cl, Message msg)
{
    if(msg.params.size() < 1)
    {
        sendReply(cl, "461 ERR_NEEDMOREPARAMS");
        return ;
    }
    if(msg.params[0][0] != '#' || (msg.params[0][0] && msg.params[0][1] == '\0'))
    {
        sendReply(cl, "476 ERR_BADCHANMASK");
        return ;
    }
    std::map<std::string, Channel>& channelList = _serv->getChannelList();
    std::map<std::string, Channel>::iterator it;
	std::string name = msg.params[0].substr(1);
    it = channelList.find(name);
    if(it != channelList.end())
    {
		if (it->second.isInviteOnly() == true) 
		{
			sendReply(cl, "473  ERR_INVITEONLYCHAN");
        	return ;
		}
		if (!it->second.getChannelKey().empty())
		{
			std::string channelKey = it->second.getChannelKey();
			if (msg.params.size() < 2 || msg.params[1] != channelKey)
			{
				sendReply(cl, "475  ERR_BADCHANNELKEY");
				return ;
			}
		}
		if (it->second.getNumberOfUsers() >= it->second.getUserLimit())
		{
			sendReply(cl, "471  ERR_CHANNELISFULL");
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
        channelList[name] = Channel(name);
    }
}

//--------MODE--------

void 	Commands::cmdMODE(client& cl, Message msg, Channel& channel) {
	if (!channel.user_present(cl))
	{
		sendReply(cl, "442  ERR_NOTONCHANNEL");
        return ;
	}
	if (msg.params.size () < 2  || msg.params[1].size() < 2)
	{
		sendReply(cl, "461 ERR_NEEDMOREPARAMS");
		return ;
	}
	char c = msg.params[1][1];
	if (msg.params[1].size() > 2 || (c != 'i' && c != 't' && c != 'k' && c != 'o' && c != 'l'))
	{
		sendReply(cl, "472 ERR_UNKNOWNMODE");
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
			sendReply(cl, "461 ERR_NEEDMOREPARAMS");
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
        sendReply(cl, "461 ERR_NEEDMOREPARAMS");
        return ;
    }
	const std::string& target = msg.params[2];
	client* recipient = _serv->findClientByNick(target);
	if (recipient == NULL)
	{
		sendReply(cl, "401 " + target + " :No such nick/channel");
		return;
	}
	std::vector<client*>::iterator it_users;
	std::vector<client*>::iterator it_operators;
	for (it_operators = channel.getOperatorsList().begin(); it_operators != channel.getOperatorsList().end(); it_operators++) {
		if (*it_operators == recipient) {
			if (msg.params[1][0] == '+')
				return;
			else if (msg.params[1][0] == '-')
			{
				channel.removeOperator(*recipient);
				return;
			}
		}
	}
	for (it_users = channel.getUserList().begin(); it_users != channel.getUserList().end(); it_users++) {
		if (*it_users == recipient) {
			if (msg.params[1][0] == '+')
			{
				channel.add_operator(*recipient);
				return;
			}
			else if (msg.params[1][0] == '-')
				return;
		}
	}
	if (it_users == channel.getUserList().end() && it_operators == channel.getOperatorsList().end()) 
	{
		sendReply(cl, "441 ERR_USERNOTINCHANNEL");
		return;
	}
}
	
void 	Commands::cmdLMODE(client&cl, Message msg, Channel &channel) {
	if (msg.params[1][0] == '+') {
		if (msg.params.size() < 3)
		{
        	sendReply(cl, "461 ERR_NEEDMOREPARAMS");
        	return ;
    	}
		long limit;
		char *end;
		limit = strtol(msg.params[2].c_str(), &end, 10);
		if (*end != '\0' || limit <= 0 || limit > INT_MAX) 
		{
        	sendReply(cl, "461 ERR_NEEDMOREPARAMS");
        	return ;
    	}
		else
			channel.setUserLimit(limit);
	}
	if (msg.params[1][0] == '-')
		channel.setUserLimit(INT_MAX);
}


//--------KICK--------
//void Commands::cmdKICK(client &cl, Message msg)
//{
//}


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
			sendReply(cl, "451 ERR_NOTREGISTERED :You have not registered");
			return;
		}
	}
	else if (cmd_exist(msg.command) == true)
	{
		if (msg.command == "PRIVMSG")
		{
			std::cout << "ouais la team" << std::endl;
			cmdPRIVMSG(cl, msg);
		}
		else if (msg.command == "JOIN")
		{
			cmdJOIN(cl, msg);
			std::cout << "parfait" << std::endl;
		}
		else if (msg.command == "MODE")
		{
			if (msg.params.empty())
			{
				sendReply(cl, "461 ERR_NEEDMOREPARAMS");
				return;
			}
			Channel* channel = getChannel(cl, msg.params[0]);
			if (channel != NULL)
				cmdMODE(cl, msg, *channel);
		}
	}
	else
	{
		sendReply(cl, "421  ERR_UNKNOWNCOMMAND");
		return;
	}
}

//DESTRUCTOR---------------------------------------------------------

Commands::~Commands() {}
