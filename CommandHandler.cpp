#include "CommandHandler.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_UNKNOWNCOMMAND ":" + hostname + " 432 " + nickname + " " + command + " :Unknown command\r\n"

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

list<string> &CommandHandler::getCommandTokens() {
	return this->_command_tokens;
}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
void CommandHandler::initializeCommandCaller() {
	this->_command_caller.insert(pair<string, ACommand *>("CAP", new Cap));
	this->_command_caller.insert(pair<string, ACommand *>("NICK", new Nick));
	this->_command_caller.insert(pair<string, ACommand *>("USER", new User));
	this->_command_caller.insert(pair<string, ACommand *>("PING", new Ping));
	this->_command_caller.insert(pair<string, ACommand *>("JOIN", new Join));
}

void CommandHandler::commandTokenizer(Server *server) {
	string token;
	istringstream iss(server->getCommandReceived());

	while (iss >> token) {
		this->_command_tokens.push_back(token);
	}

	// DEBUG PRINT LIST
	cout << "--- Elements in list: ---" << endl;
	list<string>::const_iterator it;
	int index = -1;
	it = this->_command_tokens.begin();
	for (; it != this->_command_tokens.end(); ++it) {
		std::cout << "index " << ++index << " :" << *it << std::endl;
	}
	cout << "\n" << endl;
}

string CommandHandler::sendResponse(Server *server) {
	map<string, ACommand *>::iterator it;
	string response;
	string command = this->_command_tokens.front();
	string &hostname = server->get_hostname();
	string &nickname = server->get_userDB()[server->get_client_index()]._nickname;

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
	return ERR_UNKNOWNCOMMAND;
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
