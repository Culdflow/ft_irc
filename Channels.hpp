#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "ft_irc.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "Replies.hpp"

#include <map>

class serv;

class Channel
{
	private:
		std::string 	_name;
		std::string 	_channelKey;
		std::string		_topic;
		bool			_inviteOnly;
		bool			_topicRestricted;
		unsigned int 	_userLimit;
		bool			_isLimited;
		
		std::vector<client*>    _users;
		std::vector<client*>    _operators;
		std::vector<client*>    _usersGuests;
		
	public:
		//CONSTRUCTORS
		Channel();
		Channel(const std::string& name);
		Channel(const Channel& src);
		Channel&	operator=(const Channel& src);
		//DESTRUCTORS
		~Channel();

		//METHODS
		void                add_user(client& cl);
		void                add_operator(client& cl);
		void				add_guest(client& cl);
		void                removeOperator(client& cl);
        void                removeUser(client& cl);
		bool				is_invited(client& cl);
        
		//SETTER
		void                setInviteOnly(bool value);
		void                setTopicRestricted(bool value);
		void                setChannelKey(std::string& key);
		void                setUserLimit(unsigned int u);
		void				setIsLimited(bool value);
		void                setTopic(std::string& topic);
		bool                user_present(client& cl);
		bool                isOperator(client& cl);
        
		//GETTER
		bool                isInviteOnly() const;
		std::string         getName() const;
		bool                isTopicRestricted() const;
		bool				isLimited() const;
		std::string         getChannelKey() const;
		unsigned int        getUserLimit() const;
		std::string         getTopic() const;
		std::vector<client*>& getUserList();
		std::vector<client*> getOperatorsList() const;
        unsigned int        getNumberOfUsers() const;


		void aff_users();
};


#endif