#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#pragma once

#include <exception>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string.h>

#include "ACommand.hpp"
#include "Cap.hpp"
#include "Invite.hpp"
#include "Join.hpp"
#include "Kick.hpp"
#include "Names.hpp"
#include "Nick.hpp"
#include "Part.hpp"
#include "Pass.hpp"
#include "Ping.hpp"
#include "Privmsg.hpp"
#include "User.hpp"

using std::cout;
using std::endl;
using std::istringstream;
using std::list;
using std::map;
using std::pair;
using std::string;

#define CHARACTERS_ALLOWED "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\_{|}-"

class Server;

class CommandHandler {
	public:
		// Construtors & Destructors
		CommandHandler();
		~CommandHandler();

		// Getters & Setters
		list<string>			&getCommandTokens();

		// Methods
		void					initializeCommandCaller();
		void					commandTokenizer(Server *server);
		string					sendResponse(Server *server);

	private:
		// Attributes
		map<string, ACommand *>	_command_caller;
		list<string>			_command_tokens;
};

#endif