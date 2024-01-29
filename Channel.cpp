#include "Channel.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_ALREADYINCHANNEL(channel) "400 JOIN :You are already in the channel '" + channel + "'\r\n"
#define ERR_NOTONCHANNEL(channel) "442 PART '" + channel + "' :You're not on that channel\r\n"
#define RPL_JOINCHANNEL(user, channel) ":" + user + " JOIN " + channel + "\r\n"
#define RPL_ENDOFNAMES(nickname, channel) "366 " + nickname + " " + channel + " :End of /NAMES list\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Channel::Channel(string const&name) :
	_nb_users(0), _nb_operators(0), _channel_name(name) {}

Channel::~Channel() {
	this->_user_list.clear();
}


/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */
string const &Channel::getChannelName() const {
	return this->_channel_name;
}

int const &Channel::getUsersNb() const {
	return this->_nb_users;
}

int const &Channel::getOperatorsNb() const {
	return this->_nb_operators;
}

map<int, int> &Channel::getUserList() {
	return this->_user_list;
}

bool const &Channel::getTopicRestrict() const {
	return this->_topic_restrict;
}

string const &Channel::getTopic() const {
	return this->_topic;
}

void Channel::setTopic(string const &topic) {
	this->_topic = topic;
}


bool const &Channel::getTopicRestrict() const {
	return this->_topic_restrict;
}

string const &Channel::getTopic() const {
	return this->_topic;
}

void Channel::setTopic(string const &topic) {
	this->_topic = topic;
}



/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
void Channel::addUserToChannel(Server *server, string &user, int &user_fd, int role) {
	this->_user_list[user_fd] = role;
	role == OPERATOR ? this->_nb_operators++ : this->_nb_users++;
	server->getUserDB()[user_fd]._nb_channel++;

	string msg = RPL_JOINCHANNEL(user, this->_channel_name);
	broadcastToAll(msg);
	broadcastListUser(server, user_fd);

	// DEBUG Print map
/* 	cout << "--- User in channel: ---" << this->_channel_name << endl;
	map<int, int>::const_iterator ite;
	int index = -1;
	ite = this->_user_list.begin();
	for (; ite != this->_user_list.end(); ++ite) {
		cout << "index " << ++index << " : fd= " << ite->first << " - role= " << ite->second << endl;
	}
	cout << "\n" << endl; */
}

bool Channel::isUserInChannel(int const &fd) {
	map<int, int>::const_iterator it = this->getUserList().find(fd);
	return it != this->getUserList().end();
}

void Channel::rplEndOfNames(Server *server, int &user_fd) {
	string &nickname = server->getUserDB()[user_fd]._nickname;
	string &channel = this->_channel_name;
	string msg = RPL_ENDOFNAMES(nickname, channel);
	broadcastToAll(msg);
}

void Channel::broadcastListUser(Server *server, int &user_fd) {
	const string &nickname = server->getUserDB()[user_fd]._nickname;
	string list_user = "353 " + nickname + " " + this->_channel_name + " :";
	map<int, int>::iterator it = this->_user_list.begin();
		
	for (; it != this->_user_list.end(); ++it) {
		const string &userNickname = server->getUserDB()[it->first]._nickname;
		list_user += (it->second == OPERATOR) ? ("@" + userNickname + " ") : (userNickname + " ");
	}
	list_user += "\r\n";

	broadcastToAll(list_user);
	rplEndOfNames(server, user_fd);
}

void Channel::broadcastToAll(string &msg) {
	for (map<int, int>::iterator it = this->_user_list.begin(); it != this->_user_list.end(); ++it) {
		send(it->first, msg.c_str(), msg.size(), 0);
	}
}

void Channel::removeUserFromChannel(Server *server, int &user_fd) {
	map<int, int>::iterator it = this->_user_list.find(user_fd);

	if (it != this->_user_list.end()) {
		checkRole(this, it->second);
		this->_user_list.erase(it);
		updateChannelOperator(server);
		server->getUserDB()[user_fd]._nb_channel--;
		broadcastListUser(server, user_fd);

		// DEBUG: Print updated map
/* 		cout << "--- " << server->getUserDB()[user_fd]._nickname << " has been removed from channel '" << this->_channel_name << "' ---" << endl;
		cout << "--- Updated list of users in channel '" << this->_channel_name << "' ---" << endl;
		map<int, int>::const_iterator it;
		string list_user;

		cout << "DEBUG LIST " + this->_channel_name + " :";
		it = this->_user_list.begin();
		for (; it != this->_user_list.end(); ++it) {
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
	if (!server->isChannelEmpty(this) && this->_nb_operators == 0) {
		this->_user_list.begin()->second = OPERATOR;
		this->_nb_operators++;
		this->_nb_users--;
	}
}
