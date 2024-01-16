#ifndef USER_HPP
#define USER_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>

using std::cout;
using std::endl;
using std::string;

class Server;

class User : public ACommand {
	public:
		// Construtors & Destructors
		User();
		virtual ~User();

		// Methods
		string executeCommand(Server *server);

	private:
		// Attributes
		string _name;

};

#endif