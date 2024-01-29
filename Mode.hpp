#ifndef MODE_HPP
#define MODE_HPP

#pragma once

#include "ACommand.hpp"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

class Server;

class Mode : public ACommand {
	public:
		// Construtors & Destructors
		Mode();
		virtual	~Mode();

		// Methods
		string	executeCommand(Server *server);

	private:
		// Attributes
		string	_name;
};

#endif