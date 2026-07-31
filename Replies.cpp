#include "Replies.hpp"

static std::string field(const std::string& code, const std::string& nick, const std::string& rest)
{
	return code + " " + nick + " " + rest;
}


std::string Replies::welcome(const std::string& nick)
{
	return field("001", nick, ":Welcome to the IRC network");
}


std::string Replies::notopic(const std::string& nick, const std::string& channel)
{
	return field("331", nick, channel + " :No topic is set");
}

std::string Replies::topic(const std::string& nick, const std::string& channel, const std::string& topicText)
{
	return field("332", nick, channel + " :" + topicText);
}

std::string Replies::inviting(const std::string& nick, const std::string& channel, const std::string& target)
{
	return field("341", nick, channel + " " + target);
}

std::string Replies::namreply(const std::string& nick, const std::string& channel, const std::string& names)
{
	return field("353", nick, "= " + channel + " :" + names);
}


std::string Replies::noSuchNick(const std::string& nick, const std::string& target)
{
	return field("401", nick, target + " :No such nick/channel");
}

std::string Replies::noSuchChannel(const std::string& nick, const std::string& channel)
{
	return field("403", nick, channel + " :No such channel");
}

std::string Replies::cannotSendToChan(const std::string& nick, const std::string& channel)
{
	return field("404", nick, channel + " :Cannot send to channel");
}

std::string Replies::noRecipient(const std::string& nick, const std::string& command)
{
	return field("411", nick, ":No recipient given (" + command + ")");
}

std::string Replies::noTextToSend(const std::string& nick)
{
	return field("412", nick, ":No text to send");
}

std::string Replies::unknownCommand(const std::string& nick, const std::string& command)
{
	return field("421", nick, command + " :Unknown command");
}

std::string Replies::notRegistered(const std::string& nick)
{
	return field("451", nick, ":You have not registered");
}

std::string Replies::needMoreParams(const std::string& nick, const std::string& command)
{
	return field("461", nick, command + " :Not enough parameters");
}

std::string Replies::alreadyRegistered(const std::string& nick)
{
	return field("462", nick, ":You may not reregister");
}

std::string Replies::passwdMismatch(const std::string& nick)
{
	return field("464", nick, ":Password incorrect");
}

std::string Replies::noNicknameGiven(const std::string& nick)
{
	return field("431", nick, ":No nickname given");
}

std::string Replies::erroneusNickname(const std::string& nick, const std::string& badNick)
{
	return field("432", nick, badNick + " :Erroneous nickname");
}

std::string Replies::nicknameInUse(const std::string& nick, const std::string& takenNick)
{
	return field("433", nick, takenNick + " :Nickname is already in use");
}

std::string Replies::userNotInChannel(const std::string& nick, const std::string& target, const std::string& channel)
{
	return field("441", nick, target + " " + channel + " :They aren't on that channel");
}

std::string Replies::notOnChannel(const std::string& nick, const std::string& channel)
{
	return field("442", nick, channel + " :You're not on that channel");
}

std::string Replies::userOnChannel(const std::string& nick, const std::string& target, const std::string& channel)
{
	return field("443", nick, target + " " + channel + " :is already on channel");
}

std::string Replies::badChannelKey(const std::string& nick, const std::string& channel)
{
	return field("475", nick, channel + " :Cannot join channel (+k)");
}

std::string Replies::channelIsFull(const std::string& nick, const std::string& channel)
{
	return field("471", nick, channel + " :Cannot join channel (+l)");
}

std::string Replies::unknownMode(const std::string& nick, const std::string& modeChar)
{
	return field("472", nick, modeChar + " :is unknown mode char to me");
}

std::string Replies::inviteOnlyChan(const std::string& nick, const std::string& channel)
{
	return field("473", nick, channel + " :Cannot join channel (+i)");
}

std::string Replies::badChanMask(const std::string& nick, const std::string& channel)
{
	return field("476", nick, channel + " :Bad Channel Mask");
}

std::string Replies::chanOpPrivsNeeded(const std::string& nick, const std::string& channel)
{
	return field("482", nick, channel + " :You're not channel operator");
}

std::string Relay::prefix(const std::string& nick, const std::string& user)
{
	return nick + "!" + user + "@ircserv";
}
std::string Relay::privmsg(const std::string& clientPrefix, const std::string& target, const std::string& message)
{
	return ":" + clientPrefix + " PRIVMSG " + target + " :" + message;
}
 
std::string Relay::invite(const std::string& clientPrefix, const std::string& target, const std::string& channel)
{
	return ":" + clientPrefix + " INVITE " + target + " :" + channel;
}

std::string Relay::join(const std::string& clientPrefix, const std::string& channel)
{
	return ":" + clientPrefix + " JOIN " + channel;
}

std::string Relay::topicChange(const std::string& clientPrefix, const std::string& channel, const std::string& newTopic)
{
	return ":" + clientPrefix + " TOPIC " + channel + " :" + newTopic;
}

std::string Relay::kick(const std::string& clientPrefix, const std::string& channel, const std::string& target, const std::string& reason)
{
	return ":" + clientPrefix + " KICK " + channel + " " + target + " :" + reason;
}

std::string Relay::mode(const std::string& clientPrefix, const std::string& channel, const std::string& modeStr, const std::string& modeArg)
{
	std::string line = ":" + clientPrefix + " MODE " + channel + " " + modeStr;
	if (!modeArg.empty())
		line += " " + modeArg;
	return line;
}

std::string Relay::nickChange(const std::string& oldPrefix, const std::string& newNick)
{
	return ":" + oldPrefix + " NICK :" + newNick;
}

std::string Relay::quit(const std::string& clientPrefix, const std::string& reason)
{
	return ":" + clientPrefix + " QUIT :" + reason;
}