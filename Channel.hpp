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
using std::string;
using std::reverse_iterator;

class Server;

class Channel {
	public:
		// Construtors & Destructors
		Channel(string const &name);
		~Channel();

		// Getters & Setters
		string const &getChannelName() const;
		int const &getUsersNb() const;
		int const &getOperatorsNb() const;
		map<int, int> &getUserList();
		bool const &getTopicRestrict() const;
		string const &getTopic() const;
		void setTopic(string const &topic);

		// Methods
		void addUserToChannel(Server *Server, string &user, int &user_fd, int role);
		bool isUserInChannel(int &user_fd);
		void printListUser(Server *Server);
		void rplEndOfNames(Server *server);
		void removeUserFromChannel(Server *server, int &user_fd);
		void checkRole(Channel *channel, int &role);
		void broadcastListUser(Server *server);
		void broadcastToAll(string &msg);

		// Exceptions

	private:
		// Construtors & Destructors
		Channel();
		// Attributes
		int				_nb_users;
		int				_nb_operators;
		string			_channel_name;
		string			_topic;
		bool			_topic_restrict;
		map<int, int>	_user_list;
};

#include "Server.hpp"

#endif