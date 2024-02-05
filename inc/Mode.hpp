#ifndef MODE_HPP
#define MODE_HPP

#pragma once

#include "ACommand.hpp"
#include <iostream>
#include <list>
#include <string>

#define OPERATOR 1
#define USER 2

using std::cout;
using std::endl;
using std::string;
using std::list;

class Server;
class Channel;

class Mode : public ACommand {
	public:
		// Construtors & Destructors
		Mode();
		virtual	~Mode();

		// Methods
		string	executeCommand(Server *server);
		string	parseFirstPart(Server *server, const list<string> &tokens);
		void	selectMode(Server *server, list<string>::iterator it);
		void	modeInvite(Server *server);
		void	modeTopic(Server *server);
		void	modePassword(Server *server, list<string>::iterator it);
		void	modeOperator(Server *server, list<string>::iterator it);
		string	parseOpParameter(Server *server, list<string>::iterator it, Channel *channel);
		int		changeUserMode(Server *server, int mode);
		string  sendToUser(Server *server);
        int     findToOpFd(Server *server);
		void	modeLimit(Server *server, list<string>::iterator it);
		bool	isValidChar();
		bool	isValidNumber(string &number);
		bool	checkUserStatus(Server *server, Channel *channel);
		void	modeLimitON(Server *server, Channel *channel, list<string>::iterator it);
		void	modeLimitOFF(Channel *channel);

	private:
		// Attributes
		string	_channel;
		string	_mode;
		string	_mode_param;
		string	_name;
		string	_nickname;
		string	_users_limit;
};

#endif