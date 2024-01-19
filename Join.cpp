#include "Join.hpp"
#include "Server.hpp"
#include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_ALREADYINCHANNEL(name) "400 JOIN :You are already in the channel '" + name + "'\r\n"
#define ERR_CHANNELISFULL "471 JOIN :Cannot join channel (+l)"
#define ERR_CHANNELKEYTOOLONG(key) "400 JOIN :Channel key '" + key + "' is too long (> 10 characters)\r\n"
#define ERR_CHANNELNAMETOOLONG(name) "400 JOIN :Channel name '" + name + "' is too long (> 10 characters)\r\n"
#define ERR_NEEDMOREPARAMS "461 JOIN :Not enough parameters\r\n"
#define ERR_TOOMANYCHANNELSCONNECTION "400 JOIN :Trying to connect to too many channels at the same time\r\n"
#define ERR_TOOMANYCHANNELSLIST "400 JOIN :Maximum number of channels on server has been reached\n"
#define ERR_TOOMANYKEYS "400 JOIN :Number of keys is superior to number of channels\r\n"
#define ERR_TOOMANYPARAMS "400 JOIN :Too many parameters\r\n"
#define ERR_UNKNOWNERROR(name) "400 JOIN :Missing # at the begining of channel name '" + name + "'\r\n"
#define ERR_WRONGCHARCHANNELNAME(name) "400 :Wrong characters used in name '" + name + "'\r\n"
#define ERR_WRONGCHARCHANNELKEY(key) "400 :Wrong characters used in key '" + key + "'\r\n"
#define RPL_JOINCHANNEL(user, name) ":" + user + " JOIN " + name + "\r\n"

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
	if (this->_error_msg.compare("") != 0) {
		cleanup();
		return this->_error_msg;
	}
	// 2. Get a map from the request in order to process
	createChannelMap();
	// 3. Process connections
	processChannelConnections(server);
	// 4. Clean Up
	cleanup();
	return ("");
}


//1. COMMAND PARSING
string Join::parseCommand(Server *server) {
	list<string> command = server->getCommandHandler().getCommandTokens();
	
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

string Join::parseAttributes(const list<string> &command) {
	splitParameters(command.front(), this->_channel_name);
	if (command.size() == 2) {
		splitParameters(command.back(), this->_channel_key);
	}
	if (this->_channel_name.size() > CHANLIMIT) {
		return ERR_TOOMANYCHANNELSCONNECTION;
	}
	if (this->_channel_key.size() > this->_channel_name.size()) {
		return ERR_TOOMANYKEYS;
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

//2. CREATING A MAP<CHANNEL_NAME, CHANNEL_KEY>
void Join::createChannelMap() {
	string name;
	string key;
	size_t map_size = this->_channel_name.size();

	for (size_t i = 0; i < map_size; i++) {
		if (this->_channel_key.empty()) {
			key = "";
		} else {
			key = this->_channel_key.front();
			this->_channel_key.pop_front();
		}
		this->_channel_map[this->_channel_name.front()] = key;
		this->_channel_name.pop_front();
	}

	// DEBUG Print map
	cout << "--- Elements in map ---" << endl;
	map<string, string>::const_iterator it;
	int index = -1;
	it = this->_channel_map.begin();
	for (; it != this->_channel_map.end(); ++it) {
		std::cout << "index " << ++index << " : " << it->first << " - " << it->second << std::endl;
	}
	cout << "\n" << endl;
}

//3. PROCESS CONNECTIONS
string Join::processChannelConnections(Server *server) {
	map<string, string>::const_iterator it;

	it = this->_channel_map.begin();
	for (; it != this->_channel_map.end(); ++it) {
		this->_error_msg = "";
		parseChannelNameAndKey(it->first, it->second);
		if (!this->_error_msg.empty()) {
			server->sendToClient(&this->_error_msg);
			continue;
		}
		joinChannel(server, it->first);
	}
	return "";
}

string Join::parseChannelNameAndKey(string name, string key) {
	if (name[0] != '#' && name[0] != '&' ) {
		this->_error_msg = ERR_UNKNOWNERROR(name);
	} else if (name.size() > 10) {
		this->_error_msg = ERR_CHANNELNAMETOOLONG(name);
	} else if (name.find_first_not_of(CHARACTERS_ALLOWED, 1) != string::npos) {
		this->_error_msg = ERR_WRONGCHARCHANNELNAME(name);
	} else if (key.size() > 10) {
		this->_error_msg = ERR_CHANNELKEYTOOLONG(key);
	}
	return "";
}

void Join::joinChannel(Server *server, string const &channel_name) {
	int &fd = server->getFds()[server->getClientIndex()].fd;
	string &user = server->getUserDB()[fd]._nickname;
	string error_msg;

	cout << "--- I'm: " << user << " ---\n" << endl;
	if (isChannelExisting(server, channel_name)) {
		Channel *channel = server->getChannel(channel_name);
		if (channel->isUserInChannel(fd)) {
			error_msg = ERR_ALREADYINCHANNEL(channel_name);
			server->sendToClient(&error_msg);
		} else if (channel->getUsersNb() < MAXINCHANNEL) {
			channel->addUserToChannel(server, user, fd, USER);
		} else {
			error_msg = ERR_CHANNELISFULL;
			server->sendToClient(&error_msg);
		}
	} else {
		createChannel(server, channel_name, user, fd);
	}
}

bool Join::isChannelExisting(Server *server, string const &channel_name) {
	map<string, Channel *>::const_iterator it;

	it = server->getChannelList().begin();
	for (; it != server->getChannelList().end(); ++it) {
		if (!it->first.compare(channel_name)) {
			return true;
		}
	}
	return false;
}

void Join::createChannel(Server *server, string const &channel_name, string &user, int &fd) {
	string msg;

	if (server->getChannelList().size() < MAXCHANNEL) {
		Channel *channel = new Channel(channel_name);
		server->getChannelList()[channel_name] = channel;
		channel->addUserToChannel(server, user, fd, OPERATOR);
	} else {
		msg = ERR_TOOMANYCHANNELSLIST;
		server->sendToClient(&msg);
	}
}

// 4. CLEAN UP
void Join::cleanup() {
	this->_channel_name.clear();
	this->_channel_key.clear();
	this->_channel_map.clear();
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
