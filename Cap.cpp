#include "Cap.hpp"
# include "Server.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Cap::Cap() : ACommand("CAP") {}

Cap::~Cap() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Cap::executeCommand() {
	cout << this->getCommandName() << endl;
	return ("001 user Welcome from CAP\r\n");
}
