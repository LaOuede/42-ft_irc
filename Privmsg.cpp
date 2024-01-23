# include "Privmsg.hpp"
# include "Server.hpp"
# include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */

#define PRIVMSG(nick, target, msg) ":" + nick + " PRIVMSG " + target + " " + msg + "\r\n"
// string msgPrivate = ":" + liveUser.getNickname() + " PRIVMSG " + userDestination->getNickname() + " :" + infoMessage[MSG];


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Privmsg::Privmsg() : ACommand("PRIVMSG") {}

Privmsg::~Privmsg() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Privmsg::executeCommand(Server *server) {
	list<string>::const_iterator it = server->getCommandHandler().getCommandTokens().begin();
	string target = *it;
	cout << *it << endl;
	string msg = *(++it);
	cout << *it << endl;
	string nick = server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname;
	cout << nick << endl;
	string response = PRIVMSG(nick, target, msg);
	
	if(target[0] == '#')
		for(int i = 0; i < MAXFDS ; i++)
			if(server->getFds()[i].fd != server->getFds()[server->getClientIndex()].fd)
				send(server->getFds()[i].fd, response.c_str(), response.size(), 0);
	cout << "FONCTIONNNNNNEEEEEE" << endl;
	return " ";
}

// :Angel PRIVMSG Wiz :Hello are you receiving this message