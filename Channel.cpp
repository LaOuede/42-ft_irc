#include "Channel.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_ALREADYINCHANNEL(channel) "400 JOIN :You are already in the channel '" + channel + "'\r\n"
#define ERR_NOTONCHANNEL(channel) "442 PART : '" + channel + "' :You're not on that channel\r\n"
#define RPL_JOINCHANNEL(user, channel) ":" + user + " JOIN " + channel + "\r\n"
#define RPL_ENDOFNAMES(channel) "366 " + channel + " :End of /NAMES list\r\n"


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


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
void Channel::addUserToChannel(Server *server, string &user, int &user_fd, int role) {
	this->_user_list[user_fd] = role;

	if (role == OPERATOR) {
		this->_nb_operators++;
	} else {
		this->_nb_users++;
	}
	server->getUserDB()[user_fd]._nb_channel++;
	string msg = RPL_JOINCHANNEL(user, this->_channel_name);
	server->sendToClient(msg);
	printListUser(server);

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

bool Channel::isUserInChannel(int &fd) {
	map<int, int>::const_iterator it;

	it = this->getUserList().begin();
	for (; it != this->getUserList().end(); ++it) {
		if (it->first == fd) {
			return true;
		}
	}
	return false;
}

/* When client joins the channel it receives the list of all users in this channel */
void Channel::printListUser(Server *server) {
	map<int, int>::const_iterator it;
	string list_user;

	list_user = "353 " + this->_channel_name + " :";
	it = this->_user_list.begin();
	for (; it != this->_user_list.end(); ++it) {
		string &user = server->getUserDB()[it->first]._nickname;
		if (it->second == OPERATOR) {
			list_user += "@" + user + " ";
		} else {
			list_user += user + " ";
		}
	}
	list_user += "\n";
	server->sendToClient(list_user);
	rplEndOfNames(server);
}

void Channel::rplEndOfNames(Server *server) {
	string &channel = this->_channel_name;
	string msg = RPL_ENDOFNAMES(channel);
	server->sendToClient(msg);
}

void Channel::broadcastListUser(Server *server) {
	map<int, int>::iterator it;
	map<int, int>::iterator ite;
	string list_user;

	list_user = "353 " + this->_channel_name + " :";
	it = this->_user_list.begin();
	for (; it != this->_user_list.end(); ++it) {
		string &user = server->getUserDB()[it->first]._nickname;
		if (it->second == OPERATOR) {
			list_user += "@" + user + " ";
		} else {
			list_user += user + " ";
		}
	}
	list_user += "\n";

	ite = this->_user_list.begin();
	for (; ite != this->_user_list.end(); ++ite) {
		send(ite->first, list_user.c_str(), list_user.size(), 0);
	}
}

void Channel::broadcastToAll(string &msg) {
	map<int, int>::iterator it;

	it = this->_user_list.begin();
	for (; it != this->_user_list.end(); ++it) {
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

		// DEBUG: Print updated map
		cout << "--- " << server->getUserDB()[user_fd]._nickname << " has been removed from channel '" << this->_channel_name << "' ---" << endl;
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
		cout << "\n" << endl;
	} else {
		string &channel = this->_channel_name;
		string error_msg = ERR_NOTONCHANNEL(channel);
		server->sendToClient(error_msg);
	}
}

void Channel::checkRole(Channel *channel, int &role) {
	if (role == OPERATOR) {
		channel->_nb_operators--;
	} else {
		channel->_nb_users--;
	}
}

void Channel::updateChannelOperator(Server *server) {
	if (!server->isChannelEmpty(this) && this->_nb_operators == 0) {
		map<int, int>::iterator newOper = this->_user_list.begin();
		newOper->second = OPERATOR;
		this->_nb_operators++;
		this->_nb_users--;
	}
}


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
