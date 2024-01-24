#ifndef INVITE_HPP
#define INVITE_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>
# include <list>

using std::cout;
using std::endl;
using std::string;
using std::list;

class Server;

class Invite : public ACommand {
	public:
		// Construtors & Destructors
		Invite();
		virtual	~Invite();

		// Methods
		string	executeCommand(Server *server);
		int		findClientToInvite(Server *server, const string &nickname_invited);
		string	parseFirstPart(Server *server, const list<string> &tokens, const string &channel_token);

	private:
		// Attributes
		string _name;
};

#endif