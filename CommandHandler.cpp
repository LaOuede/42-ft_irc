#include "CommandHandler.hpp"
#include "Server.hpp"

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
void CommandHandler::commandTokenizer(Server *server) {
	string token;
	istringstream iss(server->get_command_received());

	while (iss >> token) {
		_command_tokens.push_back(token);
	}

	_command_tokens.pop_front();

	// DEBUG
	std::cout << "Elements in vector:" << std::endl;
	list<std::string>::const_iterator it;
	for (it = _command_tokens.begin(); it != _command_tokens.end(); ++it) {
		std::cout << *it << std::endl;
	}

}

string CommandHandler::sendResponse(Server *server) {
	(void)server;
	map<string, ACommand *>::iterator it;

	it = this->_command_caller.find( server->get_command_received() ) ;
	if ( it != this->_command_caller.end() ) {
		return it->second->executeCommand(server) ;
	}
	return ("Command not found\n");
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
