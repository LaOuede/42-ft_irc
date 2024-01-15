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
string Cap::executeCommand(Server *server) {
	(void)server;
	cout << "Server dealing with : " << this->getCommandName() << " function" << endl;
	return ("001 user Welcome!\r\n");
}
