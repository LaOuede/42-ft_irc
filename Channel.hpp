#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#pragma once

#include <exception>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string.h>

using std::cout;
using std::endl;
using std::map;
using std::string;

class Server;

class Channel {
	public:
		// Construtors & Destructors
		Channel();
		~Channel();

		// Getters & Setters

		// Methods
		string channelCreation( Server *server);

		// Exceptions

	private:
		// Attributes
		int					_nb_users;
		int					_nb_operators;
		map<int, string>	_users_list;
		map<int, string>	_operators_list;

};

#include "Server.hpp"

#endif