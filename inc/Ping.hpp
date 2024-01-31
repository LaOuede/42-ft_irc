#ifndef PING_HPP
#define PING_HPP

#pragma once

#include "ACommand.hpp"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

class Server;

class Ping : public ACommand {
	public:
		// Construtors & Destructors
		Ping();
		virtual ~Ping();

		// Methods
		string	executeCommand(Server *server);

	private:
		// Attributes
		string	_name;
};

#endif