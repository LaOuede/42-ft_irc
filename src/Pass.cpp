#include "Pass.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_ALREADYREGISTRED "463 PRIVMSG :Unauthorized command (already registered)\r\n"
#define ERR_PASSWDMISMATCH "464 PRIVMSG :Password incorrect, you need to set the password in your client\r\n"
#define ERR_PASSWORDEMPTY "461 PASS :You need to enter a password\r\n"
#define RPL_GOOD_PASSWORD "400 PRIVMSG :The password is good\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Pass::Pass() : ACommand("PASS") {}

Pass::~Pass() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Pass::executeCommand(Server *server) {
	string &password_token = *server->getCommandHandler().getCommandTokens().begin();
	int &fd = server->getFds()[server->getClientIndex()].fd;

	if (password_token.empty())
		return (ERR_PASSWORDEMPTY);
	
	if (!isThePasswordValid(password_token, server))	
		return (ERR_PASSWDMISMATCH);
	
	server->getUserDB()[fd]._password_valid = true;
	return (RPL_GOOD_PASSWORD);
}

bool Pass::isThePasswordValid(string &password, Server *server) {
	if (password != server->getPassword())
		return (false);
	return (true);
}
