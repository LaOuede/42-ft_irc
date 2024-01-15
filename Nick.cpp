#include "Nick.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Nick::Nick() : ACommand("NICK") {}

Nick::~Nick() {}


/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */

string Nick::getCommandArgs() {
	return this->_command_args;
}

/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Nick::executeCommand(Server *server) {
	cout << this->getCommandName() << endl;
	if (this->getCommandArgs().empty())
		return ("431 user :No nickname given\r\n");
	if (!isNickValid(this->getCommandArgs()))
		return ("432 user :Erroneus nickname\r\n");
	if (!isNickInUse(this->getCommandArgs(), server)) {
		return ("433 user :Nickname is already in use\r\n");
	} else {
		server->get_userDB()[server->get_nfds()]._nickname = this->getCommandArgs();
		return (":à voir\r\n");
	}
}

bool Nick::isNickInUse(string nickname, Server *server) {
	for (map<int, clientInfo>::const_iterator it = server->get_userDB().begin(); it != server->get_userDB().end(); it++) {
		if (it->second._nickname == nickname)
			return true;
	}
	return false;
}

bool Nick::isNickValid(string nickname) {
	string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\_{|}-";

	if (nickname.empty() || nickname.length() > 9
		|| nickname.find_first_not_of(characters) != string::npos) {
		return false;
	}
	return true;
}

/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
