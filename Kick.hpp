#ifndef KICK_HPP
#define KICK_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>
# include <map>

using std::cout;
using std::endl;
using std::string;
using std::map;

class Server;

class Kick : public ACommand {
	public:
		// Construtors & Destructors
		Kick();
		virtual ~Kick();

		// Methods
		string executeCommand(Server *server);
		// string parseFirstUser(Server *server, string channel);
		// string fdToKick(Server *server, int fd_kicked, string user_kicked, map<int, int> user_list, string channel);
		// string addComment(Server *server);

	private:
		// Attributes
		string _name;

};

#endif