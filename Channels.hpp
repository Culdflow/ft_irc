/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: juliette-malaval <juliette-malaval@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 16:24:08 by juliette-ma       #+#    #+#             */
/*   Updated: 2026/07/27 16:26:23 by juliette-ma      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include "ft_irc.hpp"

class Channel
{
	private:

		//PRIVATE METHODS
		void		createSocket();
	public:
		//CONSTRUCTORS
		Channel();
		Channel(const Channel& src);
		Channel&	operator=(const Channel& src);
		//DESTRUCTORS
		~Channel();


		//METHODS
		//GETTER
};


#endif