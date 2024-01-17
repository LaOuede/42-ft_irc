#include "Nick.hpp"
#include "Server.hpp"
#include "CommandHandler.hpp"

//NICKNAME
// #define ERR_NONICKNAMEGIVEN(hostname) ":" + hostname + " 431 nonick :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(hostname) ":" + hostname + " 432 * :Erroneus nickname\r\n"
#define ERR_NICKNAMEINUSE(hostname) ":" + hostname + " 433 * :Nickname is already in use\r\n"
#define CHANGINGNICK(oldnickname, username, hostname, newnickname) ":" + oldnickname + "!" + username + "@" + hostname + " NICK " + newnickname + "\r\n"


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
	string &nickname_token = *server->get_command_handler().get_command_tokens().begin();
	string &hostname = server->get_hostname();
	string &username = server->get_userDB()[server->get_client_index()]._username;
	string &current_nickname = server->get_userDB()[server->get_client_index()]._nickname;

	if (!isNickValid(nickname_token))
		return (ERR_ERRONEUSNICKNAME(hostname));
	if (isNickInUse(nickname_token, server)) {
		return (ERR_NICKNAMEINUSE(hostname));
	}
	else if (!isNickInUse(nickname_token, server) && current_nickname.empty()) {
		current_nickname = nickname_token;
		return ("\r\n");
	}
	string old_nickname = current_nickname;
	current_nickname = nickname_token;
	return (CHANGINGNICK(old_nickname, username, hostname, nickname_token));
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
