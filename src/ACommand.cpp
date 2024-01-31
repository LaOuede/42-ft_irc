#include "ACommand.hpp"
#include "CommandHandler.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
ACommand::ACommand(string const &name) : _name(name) {}

ACommand::~ACommand() {}


/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */
string const &ACommand::getCommandName() const {
	return this->_name;
}
