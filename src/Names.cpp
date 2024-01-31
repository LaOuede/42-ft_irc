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


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Names::Names() : ACommand("NAMES") {}

Names::~Names() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Names::executeCommand(Server *server) {
	/* DEBUG À SUPPRIMER */
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

	// 2. Process connections
	printListUsers(server);

	// 3. Clean Up
	cleanup();

	return "";
}

//1. COMMAND PARSING
string Names::parseCommand(Server *server) {
	list<string> command = server->getCommandHandler().getCommandTokens();
	
	this->_error_msg = parseParameters(command);
	if (!this->_error_msg.empty()) {
		return this->_error_msg;
	}
	return parseAttributes(command);
}

string Names::parseParameters(const list<string> &command) {
	if (command.empty()) {
		return ERR_NEEDMOREPARAMS(this->_name);
	}
	if (command.size() > 1) {
		return ERR_TOOMANYPARAMS(this->_name);
	}
	return "";
}

string Names::parseAttributes(const list<string> &command) {
	splitParameters(command.front(), this->_channels_to_display);
	if (this->_channels_to_display.size() > CHANLIMIT) {
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

	for (list<string>::const_iterator it = this->_channels_to_display.begin(); it != this->_channels_to_display.end(); it++) {
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
	list<string>().swap(this->_channels_to_display);
}