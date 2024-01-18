#include "User.hpp"
#include "Server.hpp"
#include "CommandHandler.hpp"

//USER
#define ERR_NEEDMOREPARAMS "461 USER :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED "462 PRIVMSG :You may not reregister\r\n"
#define ERR_NOUSERNAME "400 :No username given\r\n"
#define ERR_WRONGCHAR "400 :Wrong characters used in username\r\n"
#define ERR_WRONGCHARREAL "400 :Wrong characters used in realname\r\n"
#define ERR_WRONGCHAR0 "400 :Supposed to be 0 after the username\r\n"
#define ERR_WRONGCHAR42 "400 :Supposed to be * after the 0\r\n"
#define ERR_WRONGCHAR3 "400 :Supposed to be : at the beginning of the realname\r\n"
#define ERR_USERTOOLONG "400 :Username too long\r\n"

#define WELCOME(hostname, nickname, username) ":" + hostname + " 001 " + nickname + " :Welcome, " + nickname + "!" + username + "@" + hostname + "\r\n"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
User::User() : ACommand("USER") {}

User::~User() {}


/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */



/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */


string User::executeCommand(Server *server) {
	list<string>::iterator it = server->getCommandHandler().getCommandTokens().begin();
	string	&username_token = *it;
	string	&hostname = server->getHostname();
	int		&fd = server->getFds()[server->getClientIndex()].fd;
	string	&nickname = server->getUserDB()[fd]._nickname;
	string	&username = server->getUserDB()[fd]._username;
	string	&realname = server->getUserDB()[fd]._realname;
	
	if (username_token.empty())
		return (defaultUser(username, realname));
	if (server->getCommandHandler().getCommandTokens().size() < 4)
		return (ERR_NEEDMOREPARAMS);
	for (; it != server->getCommandHandler().getCommandTokens().end(); it++) {

		string message_parsing = parsingUsername(*it, server);
		if (!message_parsing.empty())
			return (message_parsing);
		it++;

		message_parsing = parsingMiddleTokensAndRealname(it, server);
		if (!message_parsing.empty()) {
			return (message_parsing);
		}
		it++;

		if (it != server->getCommandHandler().getCommandTokens().end()) {
			if (!isValidChar(*it))
				return (ERR_WRONGCHARREAL);
			string old_realname = realname;
			realname = old_realname + " " + *it;
		}
		else
			break;
	}
	// cout << "--- Elements in map ---" << endl;
	// map<int, clientInfo>::const_iterator it2;
	// it2 = server->getUserDB().begin();
	// for (; it2 != server->getUserDB().end(); ++it2) {
	// 	std::cout << it2->first << ", " << it2->second._nickname << it2->second._username << it2->second._realname << std::endl;
	// }
	return (WELCOME(hostname, nickname, username));
}

string User::defaultUser(string &username, string &realname){
	username = "defaultusername" + timestamp();
	realname = "defaultrealname" + timestamp();
	return (ERR_NOUSERNAME);
}

string User::parsingUsername(string username, Server *server) {
	int &fd = server->getFds()[server->getClientIndex()].fd;
	
	if (!isValidChar(username)) {
		return (ERR_WRONGCHAR);
	}
	if (usernameTooLong(username)) {
		return (ERR_USERTOOLONG);
	}
	if (!isUserInUse(username, server)) {
		server->getUserDB()[fd]._username = username;
		return ("");
	}
	return (ERR_ALREADYREGISTRED);
}

bool User::isValidChar(string username) {
	string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\_{|}-";

	if (username.find_first_not_of(characters) != string::npos)
			return false;
	return true;
}

bool User::usernameTooLong(string username) {
	if (username.length() > 9)
		return true;
	return false;
}

bool User::isUserInUse(string username, Server *server) {
	for (map<int, clientInfo>::const_iterator it = server->getUserDB().begin(); it != server->getUserDB().end(); it++)
		if (it->second._username == username)
			return true;
	return false;
}

string User::parsingMiddleTokensAndRealname(list<string>::iterator& it, Server *server) {
	int	&fd = server->getFds()[server->getClientIndex()].fd;
	
	if (*it != "0")
		return (ERR_WRONGCHAR0);
	++it;

	if (*it != "*")
		return (ERR_WRONGCHAR42);
	++it;

	if (it->find_first_of(":") != 0)
		return (ERR_WRONGCHAR3);

	server->getUserDB()[fd]._realname = it->erase(0, 1);
	if (!isValidChar(server->getUserDB()[fd]._realname))
		return (ERR_WRONGCHARREAL);
	return ("");
}

string User::timestamp() {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[80];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, sizeof(buffer), "%d-%M-%S", timeinfo);
	string str(buffer);
	return str;
}
/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
