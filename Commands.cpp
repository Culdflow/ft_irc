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

	//debug
	std::cout << "SENDING: [" << line001 << "]" << std::endl;
	//
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

//COMMANDS-------------------------------------------------------------

void Commands::cmdPass(client& cl, Message msg) {
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

void Commands::cmdNick(client& cl, Message msg) {
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

void Commands::cmdUser(client& cl, Message msg) {
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

void Commands::cmdPrivmsg(client& cl, Message msg) {
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
        if(msg.params[0][1] == '\0')
		{
			sendReply(cl, "476 ERR_BADCHANMASK");
			return;
		}
		std::string name_chan = msg.params[0].substr(1);
		if(_serv->channel_exist(name_chan))
    	{
			// std::map<std::string, Channel> tmp = serv.getChannelList();
			std::map<std::string, Channel*>::iterator it = _serv->getChannelList().find(name_chan);
			it->second->broadcast(cl, msg);
			return;
		}
		else
		{
        	sendReply(cl, "403  ERR_NOSUCHCHANNEL");
			return;
		}
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

void Commands::cmdQuit(client& cl, Message msg) {
	if (msg.params.empty())
		sendLine(cl, "ERROR :Closing Link: Client Quit");
	else
		sendLine(cl, "ERROR :Closing Link: " + msg.params[0]);
	cl.setShouldDisconnect(true);
}

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
    std::map<std::string, Channel*>& channelList = _serv->getChannelList();
    std::map<std::string, Channel*>::iterator it;
	std::string name = msg.params[0].substr(1);
    it = channelList.find(name);
    if(it != channelList.end())
    {
		if(it->second->user_present(cl) == true)
		{
        	sendReply(cl, "443  ERR_USERONCHANNEL");
        	return ;
    	}
        it->second->add_user(cl);
    }
    else
    {
		std::cout << "salut la team\n";
        Channel *channel = new Channel(name);
        channel->add_user(cl);
        channel->add_operator(cl);
        channelList[name] = channel;
    }
}

// /mode (channel) (+ | -) (mode) [parametres]
// /mode #chez -sk CLEFPASS (enlève le mode secret et le mot de passe) 
// · t: Set/remove the restrictions of the TOPIC command to channel operators
void 	Commands::cmdMode(client& cl, Message msg) {
	if (msg.params.empty())
	{
        sendReply(cl, "461 ERR_NEEDMOREPARAMS");
        return ;
    }
	if(msg.params[0][0] != '#')
    {
        sendReply(cl, "476 ERR_BADCHANMASK");
        return ;
    }
	std::map<std::string, Channel>& channelList = _serv->getChannelList();
    std::map<std::string, Channel>::iterator it = channelList.find(msg.params[0].substr(1));
	if (it == channelList.end()) {
		sendReply(cl, "403 ERR_NOSUCHCHANNEL");
        return ;
	}
	else {
		if (msg.params.size () < 2  || msg.params[1].size() < 2)
		{
			sendReply(cl, "461 ERR_NEEDMOREPARAMS");
			return ;
		}
		char c = msg.params[1][1];
		std::cout << "debug mode : " << c << std::endl;
		if (msg.params[1].size() > 2 || (c != 'i' && c != 't' && c != 'k' && c != 'o' && c != 'l'))
		{
			sendReply(cl, "472 ERR_UNKNOWNMODE");
			return ;
    	}
		if (c == 'i')
			cmdIMode(cl, msg, it->second);
		//else if (c == 't')
			//cmdTMode(cl, msg, it->second);
		else if (c == 'k')
			cmdKMode(cl, msg, it->second);
		else if (c == 'o')
			cmdOMode(cl, msg, it->second);
		else if (c == 'l')
			cmdLMode(cl, msg, it->second);
	}
}

void 	Commands::cmdIMode(client&cl, Message msg, Channel &channel) {
	(void)cl;
	if (msg.params[1][0] == '+')
		channel.setInviteOnly(true);
	else if (msg.params[1][0] == '-')
		channel.setInviteOnly(false);
}

void 	cmdTMode(client&cl, Message msg, Channel &channel);		

void 	Commands::cmdKMode(client&cl, Message msg, Channel &channel) {
	std::string key;
	if (msg.params.size() < 3)
	{
        sendReply(cl, "461 ERR_NEEDMOREPARAMS");
        return ;
    } 
	if (msg.params[1][0] == '+')
		channel.setChannelKey(msg.params[2]); //key a verifier ? 
	else if (msg.params[1][0] == '-')
		channel.setChannelKey(key);

}

void 	Commands::cmdOMode(client&cl, Message msg, Channel &channel) {
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
	
void 	Commands::cmdLMode(client&cl, Message msg, Channel &channel) {
	if (msg.params.size() < 3)
	{
        sendReply(cl, "461 ERR_NEEDMOREPARAMS");
        return ;
    } 
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
		cmdQuit(cl, msg);
		return;
	}
	if (!cl.isUsernameSet() || !cl.isNickSet() || !cl.isPaswdCorrect())
	{
		if (msg.command == "PASS")
			cmdPass(cl, msg);
		else if (msg.command == "NICK")
			cmdNick(cl, msg);
		else if (msg.command == "USER")
			cmdUser(cl, msg);
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
			cmdPrivmsg(cl, msg);
		}
		else if (msg.command == "JOIN")
		{
			cmdJOIN(cl, msg);
			std::cout << "parfait" << std::endl;
		}
		else if (msg.command == "MODE")
			cmdMode(cl, msg);
	}
}

//DESTRUCTOR---------------------------------------------------------

Commands::~Commands() {}
