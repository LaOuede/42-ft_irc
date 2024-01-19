#include "Channel.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_ALREADYINCHANNEL(name) "400 JOIN :You are already in the channel '" + name + "'\r\n"
#define RPL_JOINCHANNEL(user, name) ":" + user + " JOIN " + name + "\r\n"

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
	this->_user_list.insert(pair<int, int>(user_fd, role));
	if (role = OPERATOR) {
		this->_nb_operators++;
	} else {
		this->_nb_users++;
	}
	string msg = RPL_JOINCHANNEL(user, this->_channel_name);
	server->sendToClient(&msg); 

	// DEBUG Print map
	cout << "--- User in channel: ---" << this->_channel_name << endl;
	map<int, int>::const_iterator ite;
	int index = -1;
	ite = this->_user_list.begin();
	for (; ite != this->_user_list.end(); ++ite) {
		std::cout << "index " << ++index << " : fd= " << ite->first << " - role= " << ite->second << std::endl;
	}
	cout << "\n" << endl;
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


/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
