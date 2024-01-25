# include "Part.hpp"
# include "Server.hpp"
# include "Channel.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_NEEDMOREPARAMS(function) "461 " + function + " :Not enough parameters\r\n"
#define ERR_NOSUCHCHANNEL(function, channel) "463 " + function + "'" + channel + "' :No such channel\r\n"
#define ERR_REASONTOOLONG(function) "400 " + function + " :Reason is too long (max. 10 characters)\r\n"
#define ERR_TOOMANYCHANNELSDECONNECTION "400 PART :Trying to deconnect from too many channels\r\n"
#define ERR_TOOMANYPARAMS(function) "400 " + function + " :Too many parameters\r\n"
#define ERR_UNKNOWNERROR(function, name) "400 " + function + " :Missing # at the begining of channel name '" + name + "'\r\n"
#define RPL_QUITCHANNEL(user, function, channel, reason) ":" + user + " " + function + " " + channel + " " + reason + "\r\n"
#define ERR_NOTONCHANNEL(channel_name) "442 PART '" + channel_name + "' :You're not on that channel\r\n"


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
	if (!this->_error_msg.empty()) {
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
	//DEBUG:
/* 	cout << "command.size() = " << command.size() << endl;

	list<string>::const_iterator it = command.begin();
	for (; it != command.end(); it++) {
		cout << *it << endl;
	} */

	if (command.empty()) {
		return ERR_NEEDMOREPARAMS(this->_name);
	}
	return "";
}

string Part::parseAttributes(const list<string> &command) {
	splitParameters(command.front(), this->_channel_name);
	if (command.size() > 1) {
		this->_reason = getReason(command);
		if (this->_reason.size() > 50) {
			return ERR_REASONTOOLONG(this->_name);
		}
	}
	return "";
}

string Part::getReason(const list<string> &command) {
	list<string>::const_iterator it = command.begin();
	string reason = *++it;
	if (reason == ":WeeChat") {
		return "";
	}
	for (it++; it != command.end(); ++it) {
		reason += " " + *it;
	}
	return reason;
}

void Part::splitParameters(string to_split, list<string> &to_fill) {
	istringstream stream(to_split);
	string token;

	while (getline(stream, token, ',')) {
		to_fill.push_back(token);
	}
}

//2. PROCESS DECONNECTIONS
string Part::processChannelDeconnections(Server *server) {
	int user_fd = server->getFds()[server->getClientIndex()].fd;

	for (list<string>::const_iterator it = _channel_name.begin(); it != _channel_name.end(); ++it) {
		const string &channel_name = *it;
		map<string, Channel *> &channel_list = server->getChannelList();
		map<string, Channel *>::const_iterator mapIt = channel_list.find(channel_name);

		if (mapIt != channel_list.end()) {
			Channel *channel = mapIt->second;
			if (channel->isUserInChannel(user_fd)) {
				broadcastUserQuitMessage(channel, server->getUserDB()[user_fd]._nickname, _reason);
				channel->removeUserFromChannel(server, user_fd);
			} else {
				server->sendToClient(ERR_NOTONCHANNEL(channel_name));
			}
		} else {
			server->sendToClient(ERR_NOSUCHCHANNEL(_name, channel_name));
		}
	}
	return "";
}

void Part::broadcastUserQuitMessage(Channel *channel, const string &user, const string &reason) {
	const string &channel_name = channel->getChannelName();
	string msg = RPL_QUITCHANNEL(user, this->getCommandName(), channel_name, reason);
	channel->broadcastToAll(msg);
}

// 3. CLEAN UP
void Part::cleanup() {
	list<string>().swap(this->_channel_name);
}
