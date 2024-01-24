# include "Privmsg.hpp"
# include "Server.hpp"
# include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */

#define PRIVMSG(nick, target, msg) ":" + nick + " PRIVMSG " + target + " " + msg + "\r\n"
#define ERR_CANNOTSENDTOCHAN(channel) "404 " + /*nickname + " " +*/ channel + " :Cannot send to channel\r\n"
#define ERR_NOSUCHNICK(target) "401 " + target + " :No such nick/channel\r\n"
#define ERR_NOTEXTTOSEND(target) "412 " + target + " :No text to send\r\n"
#define ERR_NEEDMOREPARAMS "461 PRIVMSG :Not enough parameters\r\n"


// ERR_NOTEXTTOSEND (412) 
//   "<client> :No text to send"
// ERR_NOSUCHNICK (401)
// ERR_CANNOTSENDTOCHAN (404) 
//   "<client> <channel> :Cannot send to channel"
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
	_target = *it++;
	if(_target.empty())
		return ERR_NEEDMOREPARAMS;
	string msg = *it++;
	if(msg.empty())
		return ERR_NOTEXTTOSEND(_target);
	while(it != server->getCommandHandler().getCommandTokens().end()){
		msg.append(" ");
		msg.append(*it++);
	}
	string nick = server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname;
	string response = PRIVMSG(nick, _target, msg);
	if(_target[0] == '#' || _target[0] == '&')
		return sendToChannel(server, response);
	else
		return sendToUser(server, response);
	return "";
}

int Privmsg::findTargetFd(Server *server) {
	map<int, clientInfo> &user_db = server->getUserDB();
	for (map<int, clientInfo>::const_iterator it = user_db.begin(); it != user_db.end(); ++it)
		if (it->second._nickname == _target)
			return it->first;
	return 0;
}

string Privmsg::sendToChannel(Server *server, string const& response){
	if(!server->isChannelInServer(_target))
		return ERR_NOSUCHNICK(_target);
	Channel *channel = server->getChannel(_target);
	if(!channel->isUserInChannel(server->getFds()[server->getClientIndex()].fd))
		return ERR_CANNOTSENDTOCHAN(_target);
	map<int, int>::iterator it= channel->getUserList().begin();
	for(; it != channel->getUserList().end(); it++)
		if(it->first != server->getFds()[server->getClientIndex()].fd)
			send(it->first, response.c_str(), response.size(), 0);
	return "";
}

string Privmsg::sendToUser(Server *server, string const& response){
	if(!server->isNickInServer(_target))
		return ERR_NOSUCHNICK(_target);
	send(findTargetFd(server), response.c_str(), response.size(), 0);
	return "";
}


// :Angel PRIVMSG Wiz :Hello are you receiving this message