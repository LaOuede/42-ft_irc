#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>


using std::cout;
using std::endl;
using std::string;

class Server;

class Privmsg : public ACommand {
	public:
		// Construtors & Destructors
		Privmsg();
		virtual ~Privmsg();

		// Methods
		string executeCommand(Server *server);

	private:
		// Attributes
		string _name;

};

#endif