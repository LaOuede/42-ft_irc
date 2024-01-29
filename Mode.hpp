#ifndef MODE_HPP
#define MODE_HPP

#pragma once

#include "ACommand.hpp"
#include <iostream>
#include <string>
#include <list>

#define OPERATOR 1
#define USER 2

using std::cout;
using std::endl;
using std::string;
using std::list;

class Server;

class Mode : public ACommand {
	public:
		// Construtors & Destructors
		Mode();
		virtual	~Mode();

		// Methods
		string	executeCommand(Server *server);
		string	parseFirstPart(Server *server, const list<string> &tokens);
		void	selectMode(Server *server, list<string>::iterator it);
		void	modeI(Server *server);
		void	modeT(Server *server);
		void	modeK(Server *server, list<string>::iterator it);
		void	modeO(Server *server, list<string>::iterator it);
		string  sendToUser(Server *server);
        int     findToOpFd(Server *server);
		void	modeL(Server *server, list<string>::iterator it);
		bool	isValidChar();

	private:
		// Attributes
		string	_channel;
		string	_mode;
		string	_mode_param;
		string	_name;
		string	_nickname;
		string	_target;
};

#endif