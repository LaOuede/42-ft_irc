#include "CommandHandler.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
CommandHandler::CommandHandler() {
	CommandHandler::initializeCommandCaller();
}

CommandHandler::~CommandHandler() {
	map<string, ACommand *>::iterator it;

	it = this->_command_caller.begin();
	for (; it != this->_command_caller.end(); it++ ) {
		delete it->second;
	}
	this->_command_caller.clear();
}

/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */

list<string> &CommandHandler::get_command_tokens() {
	return this->_command_tokens;
}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
void CommandHandler::initializeCommandCaller() {
	this->_command_caller.insert(pair<string, ACommand *>("CAP", new Cap));
	this->_command_caller.insert(pair<string, ACommand *>("NICK", new Nick));
	this->_command_caller.insert(pair<string, ACommand *>("USER", new User));
}

void CommandHandler::commandTokenizer(Server *server) {
	string token;
	istringstream iss(server->get_command_received());

	while (iss >> token) {
		this->_command_tokens.push_back(token);
	}

	// DEBUG PRINT LIST
/* 	cout << "--- Elements in list: ---" << endl;
	list<string>::const_iterator it;

	it = this->_command_tokens.begin();
	for (; it != this->_command_tokens.end(); ++it) {
		std::cout << *it << std::endl;
	}
	cout << "\n" << endl; */
}

string CommandHandler::sendResponse(Server *server) {
	map<string, ACommand *>::iterator it;
	string response;

	it = this->_command_caller.find(this->_command_tokens.front());
	for (; it != this->_command_caller.end(); ++it) {
		if ( it != this->_command_caller.end() ) {
			this->_command_tokens.pop_front();
			response = it->second->executeCommand(server) ;
			this->_command_tokens.clear();
			return response;
		}
	}
	this->_command_tokens.clear();
	return ("421 PRIVMSG :Command not found\n");
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
