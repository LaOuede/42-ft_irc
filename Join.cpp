#include "Join.hpp"
#include "Server.hpp"
#include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_ALREADYINCHANNEL(name) "400 JOIN :You are already in the channel '" + name + "'\r\n"
#define ERR_BADCHANNELKEY(name) "475 " + name + ":Cannot join channel (+k)\r\n"
#define ERR_CHANNELISFULL "471 JOIN :Cannot join channel (+l)\r\n"
#define ERR_CHANNELKEYTOOLONG(key) "400 JOIN :Channel key '" + key + "' is too long (> 10 characters)\r\n"
#define ERR_CHANNELNAME(name) "400 JOIN :Channel name '" + name + "' is too long (> 10 characters) or too short (<2)\r\n"
#define ERR_INVITEONLYCHAN(user, name) "473 " + user + " " + name + " :Cannot join channel (+i)\r\n"
#define ERR_NEEDMOREPARAMS(function) "461 " + function + " :Not enough parameters\r\n"
#define ERR_TOOMANYCHANNELSCONNECTION "400 JOIN :Trying to connect to too many channels at the same time\r\n"
#define ERR_TOOMANYCHANNELSLIST "400 JOIN :You have reached your maximum number of channels (5)\n"
#define ERR_TOOMANYKEYS "400 JOIN :Number of keys is superior to number of channels\r\n"
#define ERR_TOOMANYPARAMS(function) "400 " + function + " :Too many parameters\r\n"
#define ERR_UNKNOWNERROR(function, name) "400 " + function + " :Missing # at the begining of channel name '" + name + "'\r\n"
#define ERR_WELCOMED "462 PRIVMSG :You are not authenticated\r\n"
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

	// 3. Process connections
	createChannelVector();
	processChannelConnections(server);

	// 4. Clean Up
	cleanup();

	return "";
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
		return ERR_NEEDMOREPARAMS(this->_name);
	}
	if (command.size() > 2) {
		return ERR_TOOMANYPARAMS(this->_name);
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

void Join::splitParameters(string const &to_split, list<string> &to_fill) {
	istringstream stream(to_split);
	char delimiter = ',';
	string token;

	while (getline(stream, token, delimiter)) {
		to_fill.push_back(token);
	}

	// DEBUG PRINT LIST
/* 	cout << "--- Elements in " << to_split << ": ---" << endl;
	list<string>::const_iterator it;
	int index = -1;
	it = to_fill.begin();
	for (; it != to_fill.end(); ++it) {
		std::cout << "index " << ++index << " :" << *it << std::endl;
	}
	cout << to_fill.size() << endl;
	cout << "\n" << endl; */
}

//2. CREATING A VECTOR<pair<CHANNEL_NAME, CHANNEL_KEY>>
void Join::createChannelVector() {
	size_t vector_size = this->_channel_name.size();
	this->_channel_vector.reserve(vector_size);

	for (size_t i = 0; i < vector_size; i++) {
		string name = this->_channel_name.front();
		string key = this->_channel_key.empty() ? "" : this->_channel_key.front();

		this->_channel_vector.push_back(make_pair(name, key));

		this->_channel_name.pop_front();
		if (!this->_channel_key.empty()) {
			this->_channel_key.pop_front();
		}
	}

	// DEBUG Print vector
/* 	cout << "--- Elements in vector ---" << endl;
	for (size_t i = 0; i < this->_channel_vector.size(); ++i) {
	std::cout << "index " << i << " : " << this->_channel_vector[i].first << " - " << this->_channel_vector[i].second << std::endl;
	}
	cout << "\n" << endl; */
}

//3. PROCESS CONNECTIONS
string Join::processChannelConnections(Server *server) {
	int &fd = server->getFds()[server->getClientIndex()].fd;
	vector<pair<string, string> >::const_iterator it;

	it = this->_channel_vector.begin();
	for (; it != this->_channel_vector.end(); ++it) {
		this->_error_msg = "";
		parseChannelNameAndKey(it->first, it->second);
		if (!this->_error_msg.empty()) {
			server->sendToClient(this->_error_msg);
			continue;
		}
		joinChannel(server, fd, it->first, it->second);
	}
	return "";
}

string Join::parseChannelNameAndKey(string const &name, string key) {
	if (name.empty() || (name[0] != '#' && name[0] != '&')) {
		this->_error_msg = ERR_UNKNOWNERROR(this->_name, name);
		return this->_error_msg;
	}
	if (name.size() > 10 || name.size() < 2) {
		this->_error_msg = ERR_CHANNELNAME(name);
		return this->_error_msg;
	}
	if (name.find_first_not_of(CHARACTERS_ALLOWED, 1) != string::npos) {
		this->_error_msg = ERR_WRONGCHARCHANNELNAME(name);
		return this->_error_msg;
	}
	if (key.find_first_not_of(CHARACTERS_ALLOWED, 1) != string::npos) {
		this->_error_msg = ERR_WRONGCHARCHANNELKEY(key);
		return this->_error_msg;
	}
	if (!key.empty() && key.size() > 10) {
		this->_error_msg = ERR_CHANNELKEYTOOLONG(key);
		return this->_error_msg;
	}
	return "";
}

void Join::joinChannel(Server *server, int &user_fd, string const &channel_name, string const &key) {
	string &user = server->getUserDB()[user_fd]._nickname;
	int &nb_channel = server->getUserDB()[user_fd]._nb_channel;
	string error_msg;

	if (isChannelExisting(server, channel_name)) {
		Channel *channel = server->getChannel(channel_name);
		if (channel->isUserInChannel(user_fd)) {
			server->sendToClient(ERR_ALREADYINCHANNEL(channel_name));
			return;
		}
		if (channel->getUsersNb() < MAXINCHANNEL - 1 && nb_channel < MAXINCHANNEL) {
			//faire une fonction checkMode() avec Invite et Password
			if (key == channel->getPassword()) { 
				if (channel->getInviteRestrict() == false
					|| (channel->getInviteRestrict() == true && channel->isOnGuestsList(user_fd))) {
						channel->addUserToChannel(server, user, user_fd, USER);
					} else {
						server->sendToClient(ERR_INVITEONLYCHAN(user, channel_name));
					}
			} else {
				server->sendToClient(ERR_BADCHANNELKEY(channel->getChannelName()));
			}
		} else {
			server->sendToClient(ERR_CHANNELISFULL);
		}
	} else {
		createChannel(server, channel_name, user, user_fd);
	}
}

bool Join::isChannelExisting(Server *server, const string &channel_name) {
	return server->getChannelList().find(channel_name) != server->getChannelList().end();
}

void Join::createChannel(Server *server, string const &channel_name, string &user, int &fd) {
	string msg;
	int &nb_channel = server->getUserDB()[fd]._nb_channel;

	if (server->getChannelList().size() >= MAXCHANNEL && nb_channel >= MAXINCHANNEL) {
		server->sendToClient(ERR_TOOMANYCHANNELSLIST);
		return;
	}
	Channel *channel = new Channel(channel_name);
	if (!channel) {
		server->sendFailureException();
	}
	server->getChannelList()[channel_name] = channel;
	channel->addUserToChannel(server, user, fd, OPERATOR);
}

// 4. CLEAN UP
void Join::cleanup() {
	list<string>().swap(this->_channel_name);
	list<string>().swap(this->_channel_key);
	vector<pair<string, string> >().swap(this->_channel_vector);
}
