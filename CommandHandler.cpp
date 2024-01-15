#include "CommandHandler.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
CommandHandler::CommandHandler() {
	this->_command_caller[CAP] = new Cap;
	this->_command_caller[NICK] = new Nick;
	this->_command_caller[USER] = new User;
}

CommandHandler::~CommandHandler() {
	map<string, ACommand *>::iterator it;

	for ( it = this->_command_caller.begin(); it != this->_command_caller.end(); it++ ) {
		delete it->second;
	}
	this->_command_caller.clear();
}

/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */



/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string CommandHandler::sendResponse( Server *server) {
	(void)server;
	map<string, ACommand *>::iterator it;

	it = this->_command_caller.find( server->get_command_received() ) ;
	if ( it != this->_command_caller.end() ) {
		return it->second->executeCommand() ;
	}
	return ("Command not found\n");
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
