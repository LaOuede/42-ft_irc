#ifndef TOPIC_HPP
#define TOPIC_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>

using std::cout;
using std::endl;
using std::string;

class Server;

class Topic : public ACommand {
	public:
		// Construtors & Destructors
		Topic();
		virtual	~Topic();

		// Methods
		string	executeCommand(Server *server);

	private:
		// Attributes
		string _name;
};

#endif