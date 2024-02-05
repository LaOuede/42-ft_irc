#include "Nick.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_NONICKNAMEGIVEN "431 nonick :No nickname given\r\n"
#define ERR_ERRONEUSNICKNAME(nickname) "432 '" + nickname + "' :Erroneus nickname\r\n"
#define ERR_NICKNAMEINUSE(nickname) "433 '" + nickname + "' :Nickname is already in use\r\n"
#define ERR_PASSWORDNEEDED "462 PRIVMSG :You need to enter a password to set the nickname\r\n"
#define RPL_CHANGINGNICK(oldnickname, username, hostname, newnickname) ":" + oldnickname + "!" + username + "@" + hostname + " NICK " + newnickname + "\r\n"
#define RPL_NICKNAMESET(nickname) "400 :Nickname set to: '" + nickname + "'\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */

Nick::Nick() : ACommand("NICK") {}

Nick::~Nick() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */

string Nick::executeCommand(Server *server) {
	int		&fd = server->getFds()[server->getClientIndex()].fd;
	string	&nickname_token = *server->getCommandHandler().getCommandTokens().begin();
	string	&hostname = server->getHostname();
	string	&username = server->getUserDB()[fd]._username;
	string	&current_nickname = server->getUserDB()[fd]._nickname;

	if (server->getUserDB()[fd]._password_valid == false)
		return ERR_PASSWORDNEEDED;
	if (nickname_token.empty())
		return ERR_NONICKNAMEGIVEN;
	if (!isNickValid(nickname_token))
		return ERR_ERRONEUSNICKNAME(nickname_token);
	if (isNickInUse(nickname_token, server)) {
		return ERR_NICKNAMEINUSE(nickname_token);
	}
	else if (!isNickInUse(nickname_token, server) && current_nickname.empty()) {
		current_nickname = nickname_token;
		return RPL_NICKNAMESET(nickname_token);
	}
	string old_nickname = current_nickname;
	current_nickname = nickname_token;
	return RPL_CHANGINGNICK(old_nickname, username, hostname, nickname_token);
}

bool Nick::isNickInUse(string nickname, Server *server) {
	for (map<int, clientInfo>::const_iterator it = server->getUserDB().begin(); it != server->getUserDB().end(); it++)
		if (it->second._nickname == nickname)
			return true;
	return false;
}

bool Nick::isNickValid(string nickname) {
	string characters = CHARACTERS_ALLOWED;
	
	if (nickname.empty() || nickname.length() > 9
		|| nickname.find_first_not_of(characters) != string::npos || isdigit(nickname[0])) {
		return false;
	}
	return true;
}
