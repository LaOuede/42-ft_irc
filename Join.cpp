#include "Join.hpp"
#include "Server.hpp"
#include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_UNKNOWNERROR(name) "400 JOIN :Missing # at the begining of channel name '" + name + "'\r\n"
#define ERR_NEEDMOREPARAMS "461 JOIN :Not enough parameters\r\n"
#define ERR_TOOMANYPARAMS "400 JOIN :Too many parameters\r\n"
#define ERR_TOOMANYKEYS "400 JOIN :Number of keys is superior to number of channels\r\n"
#define ERR_TOOMANYCHANNELS "400 JOIN :Trying to connect to too many channels at the same time\r\n"


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
// MAIN FUNCTION
string Join::executeCommand(Server *server) {
	cout << "Server dealing with : " << this->getCommandName() << " function" << endl;

	// 1. PARSING
	this->_error_msg = parseCommand(server);
	if (this->_error_msg.compare("") != 0)
		return this->_error_msg;
	// 2. Get a map from the request in order to process
	createChannelsMap();
	// 3. Process connections
	processChannelsConnections();
	cleanup();
	return (":user!d@localhost JOIN general\r\n");
}


//1. COMMAND PARSING
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
	if (command.size() == 2) {
		splitParameters(command.back(), this->_channels_keys);
	}
	if (this->_channels_names.size() > 3) {
		return ERR_TOOMANYCHANNELS;
	}
	if (this->_channels_keys.size() > this->_channels_names.size()) {
		return ERR_TOOMANYKEYS;
	}
	return "";
}

//2. CREATING A MAP<CHANNEL_NAME, CHANNEL_KEY>
void Join::createChannelsMap() {
	string name;
	string key;
	size_t map_size = this->_channels_names.size();

	for (size_t i = 0; i < map_size; i++) {
		if (this->_channels_keys.empty()) {
			key = "";
		} else {
			key = this->_channels_keys.front();
			this->_channels_keys.pop_front();
		}
		this->_channels_map[this->_channels_names.front()] = key;
		this->_channels_names.pop_front();
	}

	// DEBUG Print map
	cout << "--- Elements in map ---" << endl;
	map<string, string>::const_iterator it;
	int index = -1;
	it = this->_channels_map.begin();
	for (; it != this->_channels_map.end(); ++it) {
		std::cout << "index " << ++index << " : " << it->first << " - " << it->second << std::endl;
	}
	cout << "\n" << endl;
}

//3. PROCESS CONNECTIONS
string Join::processChannelsConnections() {
	map<string, string>::const_iterator it;

	it = this->_channels_map.begin();
	for (; it != this->_channels_map.end(); ++it) {
		this->_error_msg = parseChannelName(it->first);
	}
	return "";
}

string Join::parseChannelName(string name) {
	if (name[0] != '#') {
		return ERR_UNKNOWNERROR(name);
	}
	return "";
}


void Join::cleanup() {
	this->_channels_names.clear();
	this->_channels_keys.clear();
}


/* 	if (token[0] != '#') {
		return ERR_UNKNOWNERROR(token);
	} */

/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
