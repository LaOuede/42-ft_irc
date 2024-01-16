#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#pragma once

#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <list>

#include "ACommand.hpp"
#include "Cap.hpp"
#include "Nick.hpp"
#include "User.hpp"

using std::cout;
using std::endl;
using std::istringstream;
using std::map;
using std::pair;
using std::string;
using std::list;

class Server;

class CommandHandler {
	public:
		// Construtors & Destructors
		CommandHandler();
		~CommandHandler();

		// Getters & Setters

		// Methods
		void initializeCommandCaller();
		void commandTokenizer(Server *server);
		string sendResponse(Server *server);

		// Exceptions

	private:
		// Attributes
		map<string, ACommand *> _command_caller;
		list<string> _command_tokens;

};

#endif