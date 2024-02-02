#include "Names.hpp"
#include "Channel.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_NEEDMOREPARAMS(function) "461 " + function + " :Not enough parameters\r\n"
#define ERR_NOSUCHCHANNEL(channel) "403 " + channel + " :No such channel\r\n"
#define ERR_NOTONCHANNEL(channel) "442 NAMES '" + channel + "' :You're not on that channel\r\n"
#define ERR_TOOMANYCHANNELSDISPLAY "400 NAMES :Trying to display users from too many channels\r\n"
#define ERR_TOOMANYPARAMS(function) "400 " + function + " :Too many parameters\r\n"
#define RPL_ENDOFNAMES(nickname, channel) "366 " + nickname + " " + channel + " :End of /NAMES list\r\n"
#define ERR_WELCOMED "462 PRIVMSG :You are not authenticated\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Names::Names() : ACommand("NAMES") {}

Names::~Names() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Names::executeCommand(Server *server) {
	if (!authentificationCheck(server)) {
		return ERR_WELCOMED;
	}
	_error_msg = parseCommand(server);
	if (!_error_msg.empty()) {
		cleanup();
		return _error_msg;
	}
	printListUsers(server);
	cleanup();
	return "";
}

//0. Authentification check
bool Names::authentificationCheck(Server *server) {
	int &fd = server->getFds()[server->getClientIndex()].fd;
	return (server->getUserDB()[fd]._welcomed == false) ? false : true;
}

//1. COMMAND PARSING
string Names::parseCommand(Server *server) {
	list<string> command = server->getCommandHandler().getCommandTokens();
	
	_error_msg = parseParameters(command);
	if (!_error_msg.empty()) {
		return _error_msg;
	}
	return parseAttributes(command);
}

string Names::parseParameters(const list<string> &command) {
	if (command.empty()) {
		return ERR_NEEDMOREPARAMS(_name);
	}
	if (command.size() > 1) {
		return ERR_TOOMANYPARAMS(_name);
	}
	return "";
}

string Names::parseAttributes(const list<string> &command) {
	splitParameters(command.front(), _channels_to_display);
	if (_channels_to_display.size() > CHANLIMIT) {
		return ERR_TOOMANYCHANNELSDISPLAY;
	}
	return "";
}

void Names::splitParameters(string const &to_split, list<string> &to_fill) {
	istringstream stream(to_split);
	char delimiter = ',';
	string token;

	while (getline(stream, token, delimiter)) {
		to_fill.push_back(token);
	}
}

// 2. PRINT CHANNELS NAMES
void Names::printListUsers(Server *server) {
	int &fd = server->getFds()[server->getClientIndex()].fd;

	for (list<string>::const_iterator it = _channels_to_display.begin(); it != _channels_to_display.end(); it++) {
		if (isChannelExisting(server, *it)) {
		Channel *channel = server->getChannel(*it);
			if (channel->isUserInChannel(fd)) {
				displayListUser(server, channel, fd);
			} else {
				server->sendToClient(ERR_NOTONCHANNEL(*it));
			}
		} else {
			server->sendToClient(ERR_NOSUCHCHANNEL(*it));
			rplEndOfNames(server, fd, *it);
		}
	}
}

bool Names::isChannelExisting(Server *server, const string &channel_name) {
	
	return server->getChannelList().find(channel_name) != server->getChannelList().end();
}

void Names::displayListUser(Server *server, Channel *channel, int &user_fd) {
	const string &nickname = server->getUserDB()[user_fd]._nickname;
	string list_user = "353 " + nickname + " " + channel->getChannelName() + " :";
	map<int, int>::iterator it = channel->getUserList().begin();
		
	for (; it != channel->getUserList().end(); ++it) {
		const string &userNickname = server->getUserDB()[it->first]._nickname;
		list_user += (it->second == OPERATOR) ? ("@" + userNickname + " ") : (userNickname + " ");
	}
	list_user += "\r\n";

	server->sendToClient(list_user);
	rplEndOfNames(server, user_fd, channel->getChannelName());
}

void Names::rplEndOfNames(Server *server, int &user_fd, string const &channel) {
	string &nickname = server->getUserDB()[user_fd]._nickname;
	server->sendToClient(RPL_ENDOFNAMES(nickname, channel));
}

// 3. CLEAN UP
void Names::cleanup() {
	list<string>().swap(_channels_to_display);
}