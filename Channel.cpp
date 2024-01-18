#include "Channel.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Channel::Channel() : _nb_users(0), _nb_operators(0), _channel_name("null") {}

Channel::~Channel() {
	this->_users_list.clear();
	this->_operators_list.clear();
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


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */



/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
