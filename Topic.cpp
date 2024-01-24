# include "Topic.hpp"
# include "Server.hpp"
# include "CommandHandler.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Topic::Topic() : ACommand("TOPIC") {}

Topic::~Topic() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Topic::executeCommand(Server *server) {
	(void)server;
	cout << "Server dealing with : " << this->getCommandName() << " function" << endl;
	return "\r\n";
}
