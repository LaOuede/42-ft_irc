#ifndef COMMANDHANDLER_HPP
#define COMMANDHANDLER_HPP

#pragma once

#include <exception>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string.h>

#include "ACommand.hpp"
#include "Cap.hpp"
#include "Nick.hpp"
#include "User.hpp"

#define CAP "CAP"
#define NICK "NICK"
#define USER "USER"

using std::cout;
using std::endl;
using std::map;
using std::string;

class Server;

class CommandHandler {
	public:
		// Construtors & Destructors
		CommandHandler();
		~CommandHandler();

		// Getters & Setters

		// Methods
		string sendResponse(Server *server);

		// Exceptions

	private:
		// Attributes
		map<string, ACommand *> _command_caller;

};

#endif