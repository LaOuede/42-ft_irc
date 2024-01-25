#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#pragma once

#include <exception>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string.h>

#define MAXINCHANNEL 5
#define OPERATOR 1
#define USER 2

using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::make_pair;
using std::exception;
using std::string;
using std::reverse_iterator;

class Server;

class Channel {
	public:
		// Construtors & Destructors
		Channel(string const &name);
		~Channel();

		// Getters & Setters
		string const	&getChannelName() const;
		int const		&getUsersNb() const;
		int const		&getOperatorsNb() const;
		map<int, int>	&getUserList();

		// Methods
		void			addUserToChannel(Server *Server, string &user, int &user_fd, int role);
		bool			isUserInChannel(int const &user_fd);
		void			rplEndOfNames(Server *server, int &user_fd);
		void			removeUserFromChannel(Server *server, int &user_fd);
		void			checkRole(Channel *channel, int &role);
		void			updateChannelOperator(Server *server);
		void			broadcastListUser(Server *server, int &user_fd);
		void			broadcastToAll(string &msg);

		// Exceptions

	private:
		// Construtors & Destructors
		Channel();
		// Attributes
		int				_nb_users;
		int				_nb_operators;
		string			_channel_name;
		map<int, int>	_user_list;
};

#include "Server.hpp"

#endif