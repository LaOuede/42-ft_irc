#include "Channel.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define RPL_ENDOFNAMES(nickname, channel) "366 " + nickname + " " + channel + " :End of /NAMES list\r\n"
#define RPL_JOINCHANNEL(user, channel) ":" + user + " JOIN " + channel + "\r\n"
#define RPL_NOTOPIC(user, channel) "331 " + user + " " + channel + " :No topic is set\r\n"
#define RPL_TOPIC(user, channel, topic) "332 " + user + " " + channel + " :" + topic + "\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Channel::Channel(string const&name) :
	_nb_users(0), _nb_operators(0), _channel_name(name), _topic_restrict(false), _password(""), _invite_restrict(false), _limit_restrict(false)  {}

Channel::~Channel() {
	_user_list.clear();
	list<int>().swap(_guests_list);
}


/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */
string const &Channel::getChannelName() const {
	return _channel_name;
}

int const &Channel::getUsersNb() const {
	return _nb_users;
}

int const &Channel::getOperatorsNb() const {
	return _nb_operators;
}

int const &Channel::getNbInChannel() const {
	return _nb_in_channel;
}

map<int, int> &Channel::getUserList() {
	return _user_list;
}

bool const &Channel::getTopicRestrict() const {
	return _topic_restrict;
}

void Channel::setTopicRestrict(bool const &topic_restrict) {
	_topic_restrict = topic_restrict;
}

string const &Channel::getTopic() const {
	return _topic;
}

void Channel::setTopic(string const &topic) {
	_topic = topic;
}

string const &Channel::getPassword() const {
	return _password;
}

void Channel::setPassword(string const &password) {
	_password = password;
}

bool const &Channel::getInviteRestrict() const {
	return _invite_restrict;
}

void Channel::setInviteRestrict(bool const &invite_restrict) {
	_invite_restrict = invite_restrict;
}

bool const &Channel::getLimitRestrict() const {
	return _limit_restrict;
}

list<int> &Channel::getGuestsList() {
	return _guests_list;
}

void Channel::setLimitRestrict(bool const &limit_restrict) {
	_limit_restrict = limit_restrict;
}

int const &Channel::getUsersLimit() const {
	return _users_limit;
}

void Channel::setUsersLimit(int const &limit) {
	_users_limit = limit;
}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
// ADD user to channel
void Channel::addUserToChannel(Server *server, string &user, int &user_fd, int role) {
	_user_list[user_fd] = role;
	role == OPERATOR ? _nb_operators++ : _nb_users++;
	_nb_in_channel++;
	server->getUserDB()[user_fd]._nb_channel++;
	updateGuestsList(user_fd, "add");
	broadcastToAll(RPL_JOINCHANNEL(user, _channel_name));
	if (!_topic.empty()) {
		server->sendToClient(RPL_TOPIC(user, _channel_name, _topic));
	} else {
		server->sendToClient(RPL_NOTOPIC(user, _channel_name));
	}
	broadcastListUser(server, user_fd);
}

void Channel::updateGuestsList(int &user_fd, string status) {
	if (status == "add") {
		if (!isOnGuestsList(user_fd)) {
			_guests_list.push_back(user_fd);
			return;
		}
	}
	if (status == "remove") {
		list<int>::const_iterator it = find(_guests_list.begin(), _guests_list.end(), user_fd);
		if (it != _guests_list.end()) {
			it = _guests_list.erase(it);
		}
	}
}

bool Channel::isOnGuestsList(int const &user_fd) {
	return find(_guests_list.begin(), _guests_list.end(), user_fd) != _guests_list.end();
}

// BROADCAST
void Channel::broadcastToAll(string msg) {
	for (map<int, int>::iterator it = _user_list.begin(); it != _user_list.end(); ++it) {
		if(send(it->first, msg.c_str(), msg.size(), 0) == -1)
			cerr << "Error : SEND return -1" << endl;
	}
}

void Channel::broadcastListUser(Server *server, int &user_fd) {
	const string &nickname = server->getUserDB()[user_fd]._nickname;
	string list_user = "353 " + nickname + " " + _channel_name + " :";
	map<int, int>::iterator it = _user_list.begin();
		
	for (; it != _user_list.end(); ++it) {
		const string &userNickname = server->getUserDB()[it->first]._nickname;
		list_user += (it->second == OPERATOR) ? ("@" + userNickname + " ") : (userNickname + " ");
	}
	list_user += "\r\n";

	broadcastToAll(list_user);
	rplEndOfNames(server, user_fd);
}

void Channel::rplEndOfNames(Server *server, int &user_fd) {
	string &nickname = server->getUserDB()[user_fd]._nickname;
	string &channel = _channel_name;
	broadcastToAll(RPL_ENDOFNAMES(nickname, channel));
}

void Channel::broadcastChannelMode(Server *server, string &nickname) {
	string msg;
	string mode_str = "+";

	if (_invite_restrict) { mode_str += "i"; }
	if (!_password.empty()) { mode_str += "k"; }
	if (_limit_restrict) { mode_str += "l"; }
	if (_topic_restrict) { mode_str += "t"; }

	if (mode_str != "+") {
		msg = "324 " + nickname + " " + _channel_name + " :" + mode_str + "\r\n";  
	} else {
		msg = "324 " + nickname + " " + _channel_name + " :\r\n";
	}
	server->sendToClient(msg);
}

// REMOVE user from channel
void Channel::removeUserFromChannel(Server *server, int &user_fd) {
	map<int, int>::iterator it = _user_list.find(user_fd);

	if (it != _user_list.end()) {
		checkRole(this, it->second);
		_nb_in_channel--;
		_user_list.erase(it);
		updateGuestsList(user_fd, "remove");
		updateChannelOperator(server);
		server->getUserDB()[user_fd]._nb_channel--;
		broadcastListUser(server, user_fd);
	}
}

void Channel::checkRole(Channel *channel, int &role) {
	(role == OPERATOR) ? _nb_operators-- : channel->_nb_users--;
}

void Channel::updateChannelOperator(Server *server) {
	if (!server->isChannelEmpty(this) && _nb_operators == 0) {
		_user_list.begin()->second = OPERATOR;
		_nb_operators++;
		_nb_users--;
	}
}

bool Channel::isUserInChannel(int const &fd) {
	map<int, int>::const_iterator it = getUserList().find(fd);
	return it != getUserList().end();
}
