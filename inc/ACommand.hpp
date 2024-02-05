#ifndef ACOMMAND_HPP
#define ACOMMAND_HPP

#pragma once

#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

class Server;

class ACommand {
	public:
		// Construtors & Destructors
		ACommand(string const &name);
		virtual ~ACommand();

		// Getters & Setters
		string const	&getCommandName() const;

		// Methods
		virtual			string executeCommand(Server *server) = 0;

	private:
		// Constructors & Destructors
		ACommand();
		ACommand(ACommand const &rhs) ;
	
		// Attributes
		string			_name;
};

#endif