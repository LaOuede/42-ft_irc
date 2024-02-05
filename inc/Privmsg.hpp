#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP

#pragma once

#include "ACommand.hpp"
#include <iostream>
#include <map>
#include <string>

using std::cerr;
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
		string	executeCommand(Server *server);
		bool	authentificationCheck(Server *server);
		string	parseParameter(Server *server);
		string	sendToChannel(Server *server);
		string	sendToUser(Server *server);
		int		findTargetFd(Server *server);

	private:
		// Attributes
		string	_name;
		string	_response;
		string	_target;
		string	_msg;
		string	_nick;
};

#endif