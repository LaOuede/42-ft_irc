#ifndef NAMES_HPP
#define NAMES_HPP

#pragma once

#include "ACommand.hpp"
#include <iostream>
#include <list>
#include <string>

using std::cout;
using std::endl;
using std::list;
using std::string;

class Server;
class Channel;

class Names : public ACommand {
	public:
		// Construtors & Destructors
		Names();
		virtual	~Names();

		// Methods
		string			executeCommand(Server *server);
		string			parseCommand(Server *server);
		string			parseParameters(const list<string> &command);
		string			parseAttributes(const list<string> &command);
		void			splitParameters(string const &to_split, list<string> &to_fill);
		void			printListUsers(Server *server);
		bool			isChannelExisting(Server *server, const string &channel_name);
		void			displayListUser(Server *server, Channel *channel, int &user_fd);
		void			rplEndOfNames(Server *server, int &user_fd, string const &channel);
		void			cleanup();

	private:
		// Attributes
		string			_name;
		string			_error_msg;
		list<string>	_channels_to_display;
};

#endif