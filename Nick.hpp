#ifndef NICK_HPP
#define NICK_HPP

#pragma once

# include "ACommand.hpp"
# include <map>
# include <iostream>
# include <string>

using std::cout;
using std::endl;
using std::string;
using std::map;

class Server;

class Nick : public ACommand {
	public:
		// Construtors & Destructors
		Nick();
		virtual ~Nick();

		// Methods
		string executeCommand(Server *server);
		bool isNickInUse(string nickname, Server *server);
		bool isNickValid(string nickname);

	private:
		// Attributes

};

#endif