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

		// Getters & Setters
		string getCommandArgs();

		// Methods
		string executeCommand(Server *server);
		bool isNickInUse(string nickname, Server *server);
		bool isNickValid(string nickname);

	private:
		// Attributes
		string _name;
		string _command_args;
		string _nickname;

};

#endif