#include "CommandHandler.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_UNKNOWNCOMMAND(nickname, command) "421 " + nickname + " " + command + " :Unknown command\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
CommandHandler::CommandHandler() {
	CommandHandler::initializeCommandCaller();
}

CommandHandler::~CommandHandler() {
	map<string, ACommand *>::iterator it;

	it = _command_caller.begin();for (; it != _command_caller.end(); it++ 
	) {
		delete it->second;
	}
	_command_caller.clear();
	list<string>().swap(_command_tokens);
}


/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */

list<string> &CommandHandler::getCommandTokens() {
	return _command_tokens;
}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
void CommandHandler::initializeCommandCaller() {
	_command_caller.insert(pair<string, ACommand *>("CAP", new Cap));
	_command_caller.insert(pair<string, ACommand *>("INVITE", new Invite));
	_command_caller.insert(pair<string, ACommand *>("JOIN", new Join));
	_command_caller.insert(pair<string, ACommand *>("KICK", new Kick));
	_command_caller.insert(pair<string, ACommand *>("MODE", new Mode));
	_command_caller.insert(pair<string, ACommand *>("NAMES", new Names));
	_command_caller.insert(pair<string, ACommand *>("NICK", new Nick));
	_command_caller.insert(pair<string, ACommand *>("PASS", new Pass));
	_command_caller.insert(pair<string, ACommand *>("PART", new Part));
	_command_caller.insert(pair<string, ACommand *>("PING", new Ping));
	_command_caller.insert(pair<string, ACommand *>("PRIVMSG", new Privmsg));
	_command_caller.insert(pair<string, ACommand *>("TOPIC", new Topic));
	_command_caller.insert(pair<string, ACommand *>("USER", new User));
	for (map<string, ACommand *>::iterator it = _command_caller.begin(); it != _command_caller.end(); it++)
		if (it->second == nullptr || it->second == NULL) {
			throw runtime_error("Fatal : New() failed");
		}
}

void CommandHandler::commandTokenizer(Server *server) {
	string token;
	istringstream iss(server->getCommandReceived());

	while (iss >> token) {
		_command_tokens.push_back(token);
	}
}

string CommandHandler::sendResponse(Server *server) {
	map<string, ACommand *>::iterator it;
	string	response;
	int		&fd = server->getFds()[server->getClientIndex()].fd;
	string	command = _command_tokens.front();
	string	&nickname = server->getUserDB()[fd]._nickname;

	it = _command_caller.find(_command_tokens.front());
	for (; it != _command_caller.end(); ++it) {
		if ( it != _command_caller.end() ) {
			_command_tokens.pop_front();
			response = it->second->executeCommand(server) ;
			_command_tokens.clear();
			return response;
		}
	}
	_command_tokens.clear();
	return ERR_UNKNOWNCOMMAND(nickname, command);
}
