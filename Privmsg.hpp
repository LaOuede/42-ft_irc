#ifndef PRIVMSG_HPP
#define PRIVMSG_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>
# include <map>


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
		int		findTargetFd(Server *server);
		string	sendToChannel(Server *server, string const& response);
		string 	sendToUser(Server *server, string const& response);

	private:
		// Attributes
		string _name;
		string _response;
		string _nick;
		string _target;
		string _msg;


};

#endif