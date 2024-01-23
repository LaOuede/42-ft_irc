# include "Kick.hpp"
# include "Server.hpp"
# include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_NEEDMOREPARAMS "461 USER :Not enough parameters\r\n"
#define ERR_NOTONCHANNEL(nickname, channel) "442 " + nickname + " " + channel + " :You're not on that channel\r\n"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Kick::Kick() : ACommand("KICK") {}

Kick::~Kick() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Kick::executeCommand(Server *server) {
	(void)server;
	// string &channel_token = *server->getCommandHandler().getCommandTokens().begin();
	// list<string>::iterator it = server->getCommandHandler().getCommandTokens().begin();
	// int		&fd = server->getFds()[server->getClientIndex()].fd;
	// Channel &channel = *server->getChannel(channel_token);

	// if (server->getCommandHandler().getCommandTokens().size() < 3)
	// 	return (ERR_NEEDMOREPARAMS);
	// /*valider que le user est dans le channel par le fd dans la list avec find*/
	// /*valider que le user est opérator par le fd dans la list avec find*/
	// it++;
	// string &nickname_kicked = *it;
	// if (nickname_kicked != /*iterator dans la db avec find et retourner le fd*/)
	// 	return (ERR_NOTONCHANNEL(nickname, channel_token));
	// if (channel.getUserList().find(fd)->first != nickname_kicked)
	// 	return (ERR_NOTONCHANNEL(nickname_kicked, channel_token));
	cout << "Server dealing with : " << this->getCommandName() << " function" << endl;
	return ("\r\n");
}
