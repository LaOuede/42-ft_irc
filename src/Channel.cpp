#include "Channel.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_ALREADYINCHANNEL(channel) "400 JOIN :You are already in the channel '" + channel + "'\r\n"
#define ERR_NOTONCHANNEL(channel) "442 PART '" + channel + "' :You're not on that channel\r\n"
#define RPL_JOINCHANNEL(user, channel) ":" + user + " JOIN " + channel + "\r\n"
#define RPL_ENDOFNAMES(nickname, channel) "366 " + nickname + " " + channel + " :End of /NAMES list\r\n"
#define RPL_TOPIC(nickname, channel, topic) "332 " + nickname + " " + channel + " :" + topic + "\r\n"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Channel::Channel(string const&name) :
	_nb_users(0), _nb_operators(0), _channel_name(name), _password(""), _invite_restrict(false) {}

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
void Channel::addUserToChannel(Server *server, string &user, int &user_fd, int role) {
	_user_list[user_fd] = role;
	role == OPERATOR ? _nb_operators++ : _nb_users++;
	_nb_in_channel++;
	server->getUserDB()[user_fd]._nb_channel++;
	updateGuestsList(user_fd, "add");
	broadcastToAll(RPL_JOINCHANNEL(user, _channel_name));
	broadcastToAll(RPL_TOPIC(user, _channel_name, _topic));
	broadcastListUser(server, user_fd);

	// DEBUG Print map
/* 	cout << "--- User in channel: ---" << _channel_name << endl;
	map<int, int>::const_iterator ite;
	int index = -1;
	ite = _user_list.begin();
	for (; ite != _user_list.end(); ++ite) {
		cout << "index " << ++index << " : fd= " << ite->first << " - role= " << ite->second << endl;
	}
	cout << "\n" << endl; */
}

bool Channel::isUserInChannel(int const &fd) {
	map<int, int>::const_iterator it = getUserList().find(fd);
	return it != getUserList().end();
}

void Channel::rplEndOfNames(Server *server, int &user_fd) {
	string &nickname = server->getUserDB()[user_fd]._nickname;
	string &channel = _channel_name;
	broadcastToAll(RPL_ENDOFNAMES(nickname, channel));
}

/* TODO Check if nickname is mandatory in message */
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

/* TODO : À revoir pour le sendFailureException() */
void Channel::broadcastToAll(string msg) {
	for (map<int, int>::iterator it = _user_list.begin(); it != _user_list.end(); ++it) {
		send(it->first, msg.c_str(), msg.size(), 0);
	}
}

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

		// DEBUG: Print updated map
/* 		cout << "--- " << server->getUserDB()[user_fd]._nickname << " has been removed from channel '" << _channel_name << "' ---" << endl;
		cout << "--- Updated list of users in channel '" << _channel_name << "' ---" << endl;
		map<int, int>::const_iterator it;
		string list_user;

		cout << "DEBUG LIST " + _channel_name + " :";
		it = _user_list.begin();
		for (; it != _user_list.end(); ++it) {
			string &user = server->getUserDB()[it->first]._nickname;
			if (it->second == OPERATOR) {
				cout << "@" + user + " ";
			} else {
				cout << user + " ";
			}
		}
		cout << "\n" << endl; */
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
