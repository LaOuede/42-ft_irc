#ifndef PASS_HPP
#define PASS_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>

using std::cout;
using std::endl;
using std::string;

class Server;

class Pass : public ACommand {
	public:
		// Construtors & Destructors
		Pass();
		virtual ~Pass();

		// Methods
		string executeCommand(Server *server);
		bool isThePasswordValid(string &password, Server *server);

	private:
		// Attributes
		string _name;

};

#endif