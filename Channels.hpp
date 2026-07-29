/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jmalaval <jmalaval@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 16:24:08 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/29 15:59:13 by jmalaval         ###   ########.fr       */
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
		std::string _name;
		
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
		void broadcast(client& cl, Message& msg);
		bool user_present(client& cl);
		//GETTER

		void aff_users();
};


#endif