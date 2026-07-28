/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jpecquer <jpecquer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 16:24:08 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/28 19:00:10 by jpecquer         ###   ########.fr       */
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
		//GETTER
};


#endif