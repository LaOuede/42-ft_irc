#include "Privmsg.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define PRIVMSG(nick, target, msg) ":" + nick + " PRIVMSG " + target + " " + msg + "\r\n"
#define ERR_CANNOTSENDTOCHAN(nickname, channel) "404 " + nickname + " " + channel + " :Cannot send to channel\r\n"
#define ERR_NOSUCHNICK(target) "401 " + target + " :No such nick/channel\r\n"
#define ERR_NOTEXTTOSEND(target) "412 " + target + " :No text to send\r\n"
#define ERR_NEEDMOREPARAMS "461 PRIVMSG :Not enough parameters\r\n"
#define ERR_WELCOMED "462 PRIVMSG :You are not authenticated\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Privmsg::Privmsg() : ACommand("PRIVMSG"){}

Privmsg::~Privmsg() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Privmsg::executeCommand(Server *server) {
	if (!authentificationCheck(server)) {
		return ERR_WELCOMED;
	}
	string status = parseParameter(server);
	if(!status.empty())
		return status ;
	_nick = server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname;
	_response = PRIVMSG(_nick, _target, _msg);
	if(_target[0] == '#' || _target[0] == '&')
		return sendToChannel(server);
	else
		return sendToUser(server);
	return "";
}

bool Privmsg::authentificationCheck(Server *server) {
    int &fd = server->getFds()[server->getClientIndex()].fd;
    return (server->getUserDB()[fd]._welcomed == false) ? false : true;
}

string Privmsg::parseParameter(Server *server){
	list<string>::const_iterator it = server->getCommandHandler().getCommandTokens().begin();
	if(it != server->getCommandHandler().getCommandTokens().end())
		_target = *it++;
	if(_target.empty())
		return ERR_NEEDMOREPARAMS;
	if(it != server->getCommandHandler().getCommandTokens().end())
		_msg = *it++;
	if(_msg.empty())
		return ERR_NOTEXTTOSEND(_target);
	while(it != server->getCommandHandler().getCommandTokens().end()){
		_msg.append(" ");
		_msg.append(*it++);
	}
	return "";
}

string Privmsg::sendToChannel(Server *server){
	if(!server->isChannelInServer(_target))
		return ERR_NOSUCHNICK(_target);
	Channel *channel = server->getChannel(_target);
	if(!channel->isUserInChannel(server->getFds()[server->getClientIndex()].fd))
		return ERR_CANNOTSENDTOCHAN(_nick, _target);
	map<int, int>::iterator it= channel->getUserList().begin();
	for(; it != channel->getUserList().end(); it++)
		if(it->first != server->getFds()[server->getClientIndex()].fd){
			if(send(it->first, _response.c_str(), _response.size(), 0) == -1)
				cerr << "Error : SEND return -1" << endl;
		}
	return "";
}

string Privmsg::sendToUser(Server *server){
	if(!server->isNickInServer(_target))
		return ERR_NOSUCHNICK(_target);
	if (send(findTargetFd(server), _response.c_str(), _response.size(), 0) == -1)
		cerr << "Error : SEND return -1" << endl;
	return "";
}

int Privmsg::findTargetFd(Server *server) {
	map<int, clientInfo> &user_db = server->getUserDB();
	for (map<int, clientInfo>::const_iterator it = user_db.begin(); it != user_db.end(); ++it)
		if (it->second._nickname == _target)
			return it->first;
	return 0;
}