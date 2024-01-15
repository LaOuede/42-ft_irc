#include "User.hpp"

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
	(void)server;
	cout << "Server dealing with : " << this->getCommandName() << " function" << endl;
	return ("001 user :you're in the shadow\r\n");
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
