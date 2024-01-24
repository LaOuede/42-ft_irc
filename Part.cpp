# include "Part.hpp"
# include "Server.hpp"
# include "Channel.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_NEEDMOREPARAMS(function) "461 " + function + " :Not enough parameters\r\n"
#define ERR_NOSUCHCHANNEL(function, channel) "463 " + function + "'" + channel + "' :No such channel\r\n"
#define ERR_REASONBADFORMAT(function) "400 " + function + " :Reason bad format (eg. ':Reason')\r\n"
#define ERR_REASONTOOLONG(function) "400 " + function + " :Reason is too long (max. 10 characters)\r\n"
#define ERR_TOOMANYCHANNELSDECONNECTION "400 PART :Trying to deconnect from too many channels\r\n"
#define ERR_TOOMANYPARAMS(function) "400 " + function + " :Too many parameters\r\n"
#define ERR_UNKNOWNERROR(function, name) "400 " + function + " :Missing # at the begining of channel name '" + name + "'\r\n"
#define RPL_QUITCHANNEL(user, function, channel, reason) ":" + user + " " + function + " " + channel + " " + reason + "\r\n"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Part::Part() : ACommand("PART") {}

Part::~Part() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Part::executeCommand(Server *server) {
	cout << "Server dealing with : " << this->getCommandName() << " function" << endl;

	// 0. Am I authentificated ?
/* 	int	&fd = server->getFds()[server->getClientIndex()].fd;
	if (server->getUserDB()[fd]._welcomed == false) {
		return (ERR_WELCOMED);
	} */
	// 1. PARSING
	this->_error_msg = parseCommand(server);
	if (this->_error_msg.compare("") != 0) {
		cleanup();
		return this->_error_msg;
	}
	// 2. Process deconnections
	processChannelDeconnections(server);
	// 3. Clean Up
	cleanup();
	return "";
}

//1. COMMAND PARSING
string Part::parseCommand(Server *server) {
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

string Part::parseParameters(const list<string> &command) {
	cout << "command.size() = " << command.size() << endl;

	list<string>::const_iterator it = command.begin();
	for (; it != command.end(); it++) {
		cout << *it << endl;
	}

	if (command.empty()) {
		return ERR_NEEDMOREPARAMS(this->_name);
	}
	if (command.size() > 2) {
		return ERR_TOOMANYPARAMS(this->_name);
	}
	return "";
}

string Part::parseAttributes(const list<string> &command) {
	splitParameters(command.front(), this->_channel_name);
	if (command.size() == 2) {
		this->_reason = command.back();
		if (this->_reason[0] != ':' || this->_reason.size() < 2 || this->_reason.find_first_not_of(CHARACTERS_ALLOWED, 1) != string::npos) {
			return ERR_REASONBADFORMAT(this->_name);
		} else if (this->_reason.size() > 10) {
			return ERR_REASONTOOLONG(this->_name);
		}
	}
	return "";
}

void Part::splitParameters(string to_split, list<string> &to_fill) {
	istringstream stream(to_split);
	char delimiter = ',';
	string token;

	while (getline(stream, token, delimiter)) {
		to_fill.push_back(token);
	}
}

//2. PROCESS DECONNECTIONS
string Part::processChannelDeconnections(Server *server) {
	list<string>::const_iterator it;
	string error_msg;
	int &user_fd = server->getFds()[server->getClientIndex()].fd;
	Channel *channel;

	it = this->_channel_name.begin();
	for (; it != this->_channel_name.end(); ++it) {
		map<string, Channel *>::const_iterator mapIt = server->getChannelList().find(*it);

		if (mapIt != server->getChannelList().end()) {
			channel = mapIt->second;
			channel->removeUserFromChannel(server, user_fd);
			if (this->_reason.empty()) {
				this->_reason = "[No specific reason mentionned]";
			}
			broadcastUserQuitMessage(channel, server->getUserDB()[user_fd]._nickname, this->_reason);
			continue;
		} else {
			error_msg = ERR_NOSUCHCHANNEL(this->_name, *it);
			server->sendToClient(error_msg);
		}
	}
	string msg = RPL_QUITCHANNEL(server->getUserDB()[user_fd]._nickname, this->getCommandName(), channel->getChannelName(), this->_reason);
	server->sendToClient(msg);
	return "";
}

void Part::broadcastUserQuitMessage(Channel *channel, const string &user, const string &reason) {
	const string &channel_name = channel->getChannelName();
	string msg = RPL_QUITCHANNEL(user, this->getCommandName(), channel_name, reason);
	channel->broadcastToAll(msg);
}

// 3. CLEAN UP
void Part::cleanup() {
	this->_channel_name.clear();
}
