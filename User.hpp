#ifndef USER_HPP
#define USER_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <string>

using std::cout;
using std::endl;
using std::string;

class Server;

class User : public ACommand {
	public:
		// Construtors & Destructors
		User();
		virtual ~User();

		// Methods
		string executeCommand(Server *server);
		bool isUserInUse(string username, Server *server);
		bool isValidChar(string username);
		bool usernameTooLong(string username);
		string parseUsername(string username, Server *server);
		string defaultUser(string &username, string &realname);
		string timestamp();

	private:
		// Attributes
		string _name;

};

#endif