#ifndef KICK_HPP
#define KICK_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>
# include <map>
# include <list>

using std::cout;
using std::endl;
using std::string;
using std::map;
using std::list;

class Server;

class Kick : public ACommand {
	public:
		// Construtors & Destructors
		Kick();
		virtual ~Kick();

		// Methods
		string	executeCommand(Server *server);
		string	parseFirstPart(Server *server, const list<string> &tokens, const string &channel_token);
		string	getComment(const list<string> &tokens);
		int		findClientToKick(Server *server, const string &user_kicked);

	private:
		// Attributes
		string _name;

};

#endif