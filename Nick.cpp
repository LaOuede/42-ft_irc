#include "Nick.hpp"
#include "Server.hpp"

//NICKNAME
#define ERR_NONICKNAMEGIVEN "431 :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(arg) "432 " + arg + " :Erroneus nickname\r\n"
#define ERR_NICKNAMEINUSE(arg) "433 " + arg + " :Nickname is already in use\r\n"
#define NEWNICK(arg) "Requesting the new nick \"" + arg + "\"\r\n"
#define CHANGINGNICK(oldarg, newarg) oldarg + " changed is nickname to " + newarg + "\r\n"

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


	// regarder s'il y a plus d'une commande
	
	if (this->getCommandArgs().empty())
		return (ERR_NONICKNAMEGIVEN);
	if (!isNickValid(this->getCommandArgs()))
		return (ERR_ERRONEUSNICKNAME(this->getCommandArgs()));
	if (!isNickInUse(this->getCommandArgs(), server) && !server->get_userDB()[server->get_client_index()]._username.empty()) {
		return (ERR_NICKNAMEINUSE(this->getCommandArgs()));
	}
	else if (!isNickInUse(this->getCommandArgs(), server) && (server->get_userDB()[server->get_client_index()]._username.empty())) {
		server->get_userDB()[server->get_client_index()]._nickname = this->getCommandArgs();
		return (NEWNICK(this->getCommandArgs()));
	}
	else if (isNickInUse(this->getCommandArgs(), server) && !server->get_userDB()[server->get_client_index()]._username.empty()) {
		string old_nickname = server->get_userDB()[server->get_client_index()]._nickname;
		server->get_userDB()[server->get_client_index()]._nickname = this->getCommandArgs();
		return (CHANGINGNICK(old_nickname, this->getCommandArgs())); //revoir le message
	}
	return ("001 user Welcome from CAP\r\n"); // pas bon
}

bool Nick::isNickInUse(string nickname, Server *server) {
	for (map<int, clientInfo>::const_iterator it = server->get_userDB().begin(); it != server->get_userDB().end(); it++)
		if (it->second._nickname == nickname)
			return true;
	return false;
}

bool Nick::isNickValid(string nickname) {
	string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\_{|}-";

	if (nickname.empty() || nickname.length() > 9
		|| nickname.find_first_not_of(characters) != string::npos || isdigit(nickname[0])) {
		return false;
	}
	return true;
}

/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
