#ifndef KICK_HPP
#define KICK_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>

using std::cout;
using std::endl;
using std::string;

class Server;

class Kick : public ACommand {
	public:
		// Construtors & Destructors
		Kick();
		virtual ~Kick();

		// Methods
		string executeCommand(Server *server);

	private:
		// Attributes
		string _name;

};

#endif