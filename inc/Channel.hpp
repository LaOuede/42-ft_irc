#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#pragma once

#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <stdexcept>
#include <string.h>

#define MAXINCHANNEL 10
#define OPERATOR 1
#define USER 2

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::list;
using std::make_pair;
using std::map;
using std::pair;
using std::reverse_iterator;
using std::string;

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
		int const		&getNbInChannel() const;
		map<int, int>	&getUserList();
		bool const		&getTopicRestrict() const;
		void			setTopicRestrict(bool const &topic_restrict);
		string const	&getTopic() const;
		void			setTopic(string const &topic);
		string const	&getPassword() const;
		void			setPassword(string const &password);
		bool const		&getInviteRestrict() const;
		void			setInviteRestrict(bool const &invite_restrict);
		bool const		&getLimitRestrict() const;
		void			setLimitRestrict(bool const &invite_restrict);
		int const		&getUsersLimit() const;
		void			setUsersLimit(int const &limit);
		list<int>		&getGuestsList();

		// Methods
		void			addUserToChannel(Server *Server, string &user, int &user_fd, int role);
		bool			isUserInChannel(int const &user_fd);
		bool			isOnGuestsList(int const &user_fd);
		void			rplEndOfNames(Server *server, int &user_fd);
		void			removeUserFromChannel(Server *server, int &user_fd);
		void			checkRole(Channel *channel, int &role);
		void			updateGuestsList(int &user_fd, string status);
		void			updateChannelOperator(Server *server);
		void			broadcastListUser(Server *server, int &user_fd);
		void			broadcastToAll(string msg);
		void			broadcastChannelMode(Server *server, string &nickname);

	private:
		// Construtors & Destructors
		Channel();
	
		// Attributes
		int				_nb_users;
		int				_nb_operators;
		int				_nb_in_channel;
		string			_channel_name;
		string			_topic;
		bool			_topic_restrict;
		map<int, int>	_user_list;
		string			_password;
		bool			_invite_restrict;
		bool			_limit_restrict;
		int				_users_limit;
		list<int>		_guests_list;
};

#include "Server.hpp"

#endif