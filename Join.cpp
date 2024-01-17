#include "Join.hpp"
#include "Server.hpp"
#include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_UNKNOWNERROR(token) "400 JOIN :Missing # at the begining of channel name '" + token + "'\r\n"
#define ERR_NEEDMOREPARAMS "461 JOIN :Not enough parameters\r\n"
#define ERR_TOOMANYPARAMS "400 JOIN :Too many parameters\r\n"
#define ERR_BADPARAMS "400 JOIN :Number of keys is superior to number of channels\r\n"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Join::Join() : ACommand("Join") {}

Join::~Join() {}


/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */



/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Join::executeCommand(Server *server) {
	cout << "Server dealing with : " << this->getCommandName() << " function" << endl;

	this->_error_msg = parseCommand(server);
	if (this->_error_msg.compare("") != 0)
		return this->_error_msg;
	return (":user!d@localhost JOIN general\r\n");
}

string Join::parseCommand(Server *server) {
	list<string> command = server->get_command_handler().get_command_tokens();
	
	this->_error_msg = parseParameters(command);
	if (!this->_error_msg.empty()) {
		return this->_error_msg;
	}
	this->_error_msg = parseAttributes(command);
	if (!this->_error_msg.empty()) {
		return this->_error_msg;
	}
/* 	if (token[0] != '#') {
		return ERR_UNKNOWNERROR(token);
	} */
	return "";
}

string Join::parseParameters(const list<string> &command) {
	if (command.empty()) {
		return ERR_NEEDMOREPARAMS;
	}
	if (command.size() > 2) {
		return ERR_TOOMANYPARAMS;
	}
	return "";
}

void Join::splitParameters(string to_split, list<string> &to_fill) {
	istringstream stream(to_split);
	char delimiter = ',';
	string token;

	while (getline(stream, token, delimiter)) {
		to_fill.push_back(token);
	}

	// DEBUG PRINT LIST
	cout << "--- Elements in " << to_split << ": ---" << endl;
	list<string>::const_iterator it;
	int index = -1;
	it = to_fill.begin();
	for (; it != to_fill.end(); ++it) {
		std::cout << "index " << ++index << " :" << *it << std::endl;
	}
	cout << to_fill.size() << endl;
	cout << "\n" << endl;
}

string Join::parseAttributes(const list<string> &command) {
	splitParameters(command.front(), this->_channels_names);
	splitParameters(command.back(), this->_channels_keys);

	if (this->_channels_keys.size() > this->_channels_names.size()) {
		return ERR_BADPARAMS;
	}
	return "";
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
