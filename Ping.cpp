# include "Ping.hpp"
# include "Server.hpp"
# include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_NEEDMOREPARAMS "461 USER :Not enough parameters\r\n"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Ping::Ping() : ACommand("PING") {}

Ping::~Ping() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Ping::executeCommand(Server *server) {
	string &ping_token = *server->getCommandHandler().getCommandTokens().begin();

	if (ping_token.empty())
		return ERR_NEEDMOREPARAMS;
	return "PONG " + ping_token + "\r\n";
}
