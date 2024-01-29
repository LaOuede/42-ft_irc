#include "Mode.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Mode::Mode() : ACommand("MODE") {}

Mode::~Mode() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Mode::executeCommand(Server *server) {
	(void)server;
	cout << "Server dealing with : " << this->getCommandName() << " function" << endl;
	return "\r\n";
}
