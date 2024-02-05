#ifndef INVITE_HPP
#define INVITE_HPP

#pragma once

#include "ACommand.hpp"
#include <iostream>
#include <list>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::string;

class Server;

class Invite : public ACommand {
	public:
		// Construtors & Destructors
		Invite();
		virtual ~Invite();

		// Methods
		string	executeCommand(Server *server);
		bool	authentificationCheck(Server *server);
		int		findClientToInvite(Server *server, const string &nickname_invited);
		string	parseFirstPart(Server *server, const string &channel_token);
		bool	isClientInvited(Server *server, int fd, string channel_token);

	private:
		// Attributes
		string	_name;
		string	_nickname;
		string	_channel;
		string	_invited;
		int		_fd_invited;
};

#endif
