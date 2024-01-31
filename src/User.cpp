#include "User.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define USERNAMESET(username, realname) "400 :Username set to: '" + username + "' and the realname set to: '" + realname + "'\r\n"
#define ERR_NEEDMOREPARAMS "461 USER :Not enough parameters\r\n"
#define ERR_ALREADYREGISTRED "462 PRIVMSG :You may not reregister\r\n"
#define ERR_NOUSERNAME "400 :No username given\r\n"
#define ERR_WRONGCHAR "400 :Wrong characters used in username\r\n"
#define ERR_WRONGCHARREAL "400 :Wrong characters used in realname\r\n"
#define ERR_WRONGCHAR0 "400 :Supposed to be 0 after the username\r\n"
#define ERR_WRONGCHAR42 "400 :Supposed to be * after the 0\r\n"
#define ERR_WRONGCHAR3 "400 :Supposed to be : at the beginning of the realname\r\n"
#define ERR_USERTOOLONG "400 :Username too long\r\n"
#define ERR_PASSWORDNEEDED "462 PRIVMSG :You need to enter a password to set the username\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
User::User() : ACommand("USER") {}

User::~User() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string User::executeCommand(Server *server) {
	list<string>::iterator it = server->getCommandHandler().getCommandTokens().begin();
	string	&username_token = *it;
	int		&fd = server->getFds()[server->getClientIndex()].fd;
	string	&username = server->getUserDB()[fd]._username;
	string	&realname = server->getUserDB()[fd]._realname;
	
	if (server->getUserDB()[fd]._password_valid == false)
		return ERR_PASSWORDNEEDED;

	if (username_token.empty())
		return defaultUser(username, realname);

	if (server->getCommandHandler().getCommandTokens().size() < 4)
		return ERR_NEEDMOREPARAMS;
	string temp_username;
	while (it != server->getCommandHandler().getCommandTokens().end()) {
		string message_parsing = parsingUsername(*it, server);
		temp_username = *it;
		if (!message_parsing.empty())
			return message_parsing;
		it++;

		message_parsing = parsingMiddleTokensAndRealname(it, server);
		if (!message_parsing.empty()){
			return message_parsing;}
		it++;

		if (it != server->getCommandHandler().getCommandTokens().end()) {
			if (!isValidChar(*it))
				return ERR_WRONGCHARREAL;
			realname += " " + *it;
			it = server->getCommandHandler().getCommandTokens().end();
		}
		else
			break;
	}
	server->getUserDB()[fd]._username = temp_username;
	// cout << "--- Elements in map ---" << endl;
	// map<int, clientInfo>::const_iterator it2;
	// it2 = server->getUserDB().begin();
	// for (; it2 != server->getUserDB().end(); ++it2) {
	// 	std::cout << it2->first << ", " << it2->second._nickname << it2->second._username << it2->second._realname << std::endl;
	// }
	return USERNAMESET(username, realname);
}

string User::defaultUser(string &username, string &realname){
	username = "defaultusername" + timestamp();
	realname = "defaultrealname" + timestamp();
	return ERR_NOUSERNAME;
}

string User::parsingUsername(string username, Server *server) {
	if (!isValidChar(username))
		return ERR_WRONGCHAR;

	if (usernameTooLong(username))
		return ERR_USERTOOLONG;

	if (!isUserInUse(username, server)) {
		return "";
	}
	return ERR_ALREADYREGISTRED;
}

bool User::isValidChar(string username) {
	string characters = CHARACTERS_ALLOWED;

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
		return ERR_WRONGCHAR0;
	++it;

	if (*it != "*")
		return ERR_WRONGCHAR42;
	++it;

	if (it->find_first_of(":") != 0)
		return ERR_WRONGCHAR3;

	if (!isValidChar(it->erase(0, 1)))
		return ERR_WRONGCHARREAL;
	server->getUserDB()[fd]._realname = *it;
	return "";
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
