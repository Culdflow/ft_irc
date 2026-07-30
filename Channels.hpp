/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliette-malaval <juliette-malaval@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 16:24:08 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/30 16:17:27 by juliette-ma      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "ft_irc.hpp"
#include "Client.hpp"
#include "Server.hpp"
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
		
		std::vector<client*>    _users;
		std::vector<client*>    _operators;
		
	public:
		//CONSTRUCTORS
		Channel();
		Channel(const std::string& name);
		Channel(const Channel& src);
		Channel&	operator=(const Channel& src);
		//DESTRUCTORS
		~Channel();

		//METHODS
		void add_user(client& cl);
		void add_operator(client& cl);
		void removeOperator(client& cl);
		//SETTER
		void setInviteOnly(bool value);
		void setTopicRestricted(bool value);
		void setChannelKey(std::string& key);
		void setUserLimit(unsigned int u);
		void setTopic(std::string& topic);
		void broadcast(client& cl, Message& msg);
		bool user_present(client& cl);
		//GETTER
		bool isInviteOnly() const;
		bool isTopicRestricted() const;
		std::string getChannelKey() const;
		unsigned int getUserLimit() const;
		std::string getTopic() const;
		std::vector<client*> getUserList() const;
		std::vector<client*> getOperatorsList() const;
        unsigned int getNumberOfUsers() const;


		void aff_users();
};


#endif