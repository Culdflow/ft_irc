#ifndef REPLIES_HPP
# define REPLIES_HPP

# include <string>

class Replies
{
	public:
		static std::string welcome(const std::string& nick);
		static std::string yourHost(const std::string& nick);
		static std::string created(const std::string& nick);
		static std::string myInfo(const std::string& nick);
		static std::string isSupport(const std::string& nick);

		static std::string notopic(const std::string& nick, const std::string& channel);
		static std::string topic(const std::string& nick, const std::string& channel, const std::string& topicText);
		static std::string inviting(const std::string& nick, const std::string& channel, const std::string& target);
		static std::string namreply(const std::string& nick, const std::string& channel, const std::string& names);

		static std::string noSuchNick(const std::string& nick, const std::string& target);
		static std::string noSuchChannel(const std::string& nick, const std::string& channel);
		static std::string noRecipient(const std::string& nick, const std::string& command);
		static std::string noTextToSend(const std::string& nick);
		static std::string unknownCommand(const std::string& nick, const std::string& command);
		static std::string notRegistered(const std::string& nick);
		static std::string needMoreParams(const std::string& nick, const std::string& command);
		static std::string alreadyRegistered(const std::string& nick);
		static std::string passwdMismatch(const std::string& nick);
		static std::string noNicknameGiven(const std::string& nick);
		static std::string erroneusNickname(const std::string& nick, const std::string& badNick);
		static std::string nicknameInUse(const std::string& nick, const std::string& takenNick);
		static std::string userNotInChannel(const std::string& nick, const std::string& target, const std::string& channel);
		static std::string notOnChannel(const std::string& nick, const std::string& channel);
		static std::string userOnChannel(const std::string& nick, const std::string& target, const std::string& channel);
		static std::string badChannelKey(const std::string& nick, const std::string& channel);
		static std::string channelIsFull(const std::string& nick, const std::string& channel);
		static std::string unknownMode(const std::string& nick, const std::string& modeChar);
		static std::string inviteOnlyChan(const std::string& nick, const std::string& channel);
		static std::string badChanMask(const std::string& nick, const std::string& channel);
		static std::string chanOpPrivsNeeded(const std::string& nick, const std::string& channel);
		static std::string channelModeIs(const std::string& nick, const std::string& channel, const std::string &mode);

	private:
		Replies();
};


class Relay
{
	public:
		static std::string prefix(const std::string& nick, const std::string& user);

		static std::string privmsg(const std::string& clientPrefix, const std::string& target, const std::string& message);
		static std::string invite(const std::string& clientPrefix, const std::string& target, const std::string& channel);
        static std::string join(const std::string& clientPrefix, const std::string& channel);
		static std::string topicChange(const std::string& clientPrefix, const std::string& channel, const std::string& newTopic);
		static std::string kick(const std::string& clientPrefix, const std::string& channel, const std::string& target, const std::string& reason);
		static std::string mode(const std::string& clientPrefix, const std::string& channel, const std::string& modeStr, const std::string& modeArg);
		static std::string nickChange(const std::string& oldPrefix, const std::string& newNick);
		static std::string quit(const std::string& clientPrefix, const std::string& reason);

	private:
		Relay();
};

#endif