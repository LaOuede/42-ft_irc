#include "Join.hpp"
#include "Server.hpp"
#include "Channel.hpp"

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
#define ERR_TOOMANYCHANNELSSERVER "400 JOIN :Server has reached maximum number of channels (10)\n"
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
	if (!authentificationCheck(server)) {
		return ERR_WELCOMED;
	}
	_error_msg = parseCommand(server);
	if (!_error_msg.empty()) {
		cleanup();
		return _error_msg;
	}
	createChannelVector();
	processChannelConnections(server);
	cleanup();
	return "";
}

//0. Authentification check
bool Join::authentificationCheck(Server *server) {
    int &fd = server->getFds()[server->getClientIndex()].fd;
    return (server->getUserDB()[fd]._welcomed == false) ? false : true;
}

//1. COMMAND PARSING
string Join::parseCommand(Server *server) {
	_command = server->getCommandHandler().getCommandTokens();
	
	_error_msg = parseParameters();
	if (!_error_msg.empty()) {
		return _error_msg;
	}
	_error_msg = parseAttributes();
	if (!_error_msg.empty()) {
		return _error_msg;
	}
	return "";
}

string Join::parseParameters() {
	if (_command.empty()) {
		return ERR_NEEDMOREPARAMS(_name);
	}
	if (_command.size() > 2) {
		return ERR_TOOMANYPARAMS(_name);
	}
	return "";
}

string Join::parseAttributes() {
	splitParameters(_command.front(), _channel_name);
	if (_command.size() == 2) {
		splitParameters(_command.back(), _channel_key);
	}
	if (_channel_name.size() > CHANLIMIT) {
		return ERR_TOOMANYCHANNELSCONNECTION;
	}
	if (_channel_key.size() > _channel_name.size()) {
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
	size_t vector_size = _channel_name.size();
	_channel_vector.reserve(vector_size);

	for (size_t i = 0; i < vector_size; i++) {
		string name = _channel_name.front();
		string key = _channel_key.empty() ? "" : _channel_key.front();

		_channel_vector.push_back(make_pair(name, key));

		_channel_name.pop_front();
		if (!_channel_key.empty()) {
			_channel_key.pop_front();
		}
	}

	// DEBUG Print vector
/* 	cout << "--- Elements in vector ---" << endl;
	for (size_t i = 0; i < _channel_vector.size(); ++i) {
	std::cout << "index " << i << " : " << _channel_vector[i].first << " - " << _channel_vector[i].second << std::endl;
	}
	cout << "\n" << endl; */
}

//3. PROCESS CONNECTIONS
string Join::processChannelConnections(Server *server) {
	int &fd = server->getFds()[server->getClientIndex()].fd;
	vector<pair<string, string> >::const_iterator it;

	it = _channel_vector.begin();
	for (; it != _channel_vector.end(); ++it) {
		_error_msg = "";
		parseChannelNameAndKey(it->first, it->second);
		if (!_error_msg.empty()) {
			server->sendToClient(_error_msg);
			continue;
		}
		joinChannel(server, fd, it->first, it->second);
	}
	return "";
}

string Join::parseChannelNameAndKey(string const &name, string key) {
	if (name.empty() || (name[0] != '#' && name[0] != '&')) {
		_error_msg = ERR_UNKNOWNERROR(_name, name);
		return _error_msg;
	}
	if (name.size() > 10 || name.size() < 2) {
		_error_msg = ERR_CHANNELNAME(name);
		return _error_msg;
	}
	if (name.find_first_not_of(CHARACTERS_ALLOWED, 1) != string::npos) {
		_error_msg = ERR_WRONGCHARCHANNELNAME(name);
		return _error_msg;
	}
	if (key.find_first_not_of(CHARACTERS_ALLOWED, 1) != string::npos) {
		_error_msg = ERR_WRONGCHARCHANNELKEY(key);
		return _error_msg;
	}
	if (!key.empty() && key.size() > 10) {
		_error_msg = ERR_CHANNELKEYTOOLONG(key);
		return _error_msg;
	}
	return "";
}

void Join::joinChannel(Server *server, int &user_fd, string const &channel_name, string const &key) {
	string &user = server->getUserDB()[user_fd]._nickname;

	if (server->isChannelInServer(channel_name)) {
		Channel *channel = server->getChannel(channel_name);
		if (channel->isUserInChannel(user_fd)) {
			server->sendToClient(ERR_ALREADYINCHANNEL(channel_name));
			return;
		}
		if (checkMode(server, channel, key, user, user_fd, channel_name)) {
			channel->addUserToChannel(server, user, user_fd, USER);

		}
	} else {
		createChannel(server, channel_name, user, user_fd);
	}
}

bool Join::checkMode(Server *server, Channel *channel, string key, string &user, int &user_fd, string channel_name) {
	if ((channel->getLimitRestrict() == true && channel->getNbInChannel() >= channel->getUsersLimit())
		|| (channel->getLimitRestrict() == false && channel->getNbInChannel() >= MAXINCHANNEL)) {
		server->sendToClient(ERR_CHANNELISFULL);
		return false;
	}
	if (key != channel->getPassword()) { 
		server->sendToClient(ERR_BADCHANNELKEY(channel->getChannelName()));
		return false;
	}
	if (channel->getInviteRestrict() == true && !channel->isOnGuestsList(user_fd)) {
		server->sendToClient(ERR_INVITEONLYCHAN(user, channel_name));
		return false;
	}
	return true;
}

void Join::createChannel(Server *server, string const &channel_name, string &user, int &fd) {
	if (!checkChannelsLimits(server, fd)) {
		return;
	}

	Channel *channel = new Channel(channel_name);
	if (!channel) {
		server->sendFailureException();
	}
	server->getChannelList()[channel_name] = channel;
	channel->addUserToChannel(server, user, fd, OPERATOR);
}

bool Join::checkChannelsLimits(Server *server, int &fd) {
	int &nb_channel = server->getUserDB()[fd]._nb_channel;
	cout << "nb channel I'm is: " << nb_channel << endl;
	if (server->getChannelList().size() >= MAXCHANNEL) {
		server->sendToClient(ERR_TOOMANYCHANNELSSERVER);
		return false;
	}
	if (nb_channel >= CHANLIMIT) {
		server->sendToClient(ERR_TOOMANYCHANNELSLIST);
		return false;
	}
	return true;
}

// 4. CLEAN UP
void Join::cleanup() {
	list<string>().swap(_command);
	list<string>().swap(_channel_name);
	list<string>().swap(_channel_key);
	vector<pair<string, string> >().swap(_channel_vector);
}
