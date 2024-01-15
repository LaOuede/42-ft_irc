#ifndef CAP_HPP
#define CAP_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>

using std::cout;
using std::endl;
using std::string;

class Server;

class Cap : public ACommand {
	public:
		// Construtors & Destructors
		Cap();
		virtual ~Cap();

		// Methods
		string executeCommand(Server *server);

	private:
		// Attributes
		string _name;

};

#endif