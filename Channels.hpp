/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmalaval <jmalaval@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 16:24:08 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/29 17:56:33 by jmalaval         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "ft_irc.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include <map>

class Channel
{
	private:
		std::string 	_name;
		std::string 	_channelKey;
		std::string		_topic;
		bool			_inviteOnly;
		unsigned int 	_userLimit;
		
		std::vector<client*> _users;
		std::vector<client*> _operators;
		
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
		void setChannelKey(std::string& key);
		void setUserLimit(unsigned int u);
		void setTopic(std::string& topic);
		//GETTER
		bool isInviteOnly() const;
		std::string getChannelKey() const;
		unsigned int getUserLimit() const;
		std::string getTopic() const;
		std::vector<client*> getUserList() const;
		std::vector<client*> getOperatorsList() const;

};


#endif