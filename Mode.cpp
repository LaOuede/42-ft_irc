#include "Mode.hpp"
#include "Server.hpp"
#include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_NEEDMOREPARAMS(nickname) "461 " + nickname + " MODE :Not enough parameters\r\n"
#define ERR_WELCOMED "462 MODE :You are not authenticated\r\n"
#define ERR_NOSUCHCHANNEL(channel) "403 " + channel + " :No such channel\r\n"
#define ERR_PASSWDMISMATCH "464 PRIVMSG :Password incorrect\r\n"
#define ERR_WRONGCHAR "400 :Wrong character or too long password\r\n"
#define ERR_CHANOPRIVSNEEDED(nickname, channel) "482 " + nickname + " " + channel + " :You're not channel operator\r\n"
#define ERR_WRONGPARAMS(nickname) "461 " + nickname + " MODE :Syntax error\r\n"
#define ERR_NOSUCHNICK(target) "401 " + target + " :No such nick\r\n"
#define CLIENTOPTARGET(nickname, target) nickname + " :set " + target + " operator mode\r\n"
#define TARGETOPBYCLIENT(nickname) "You was set operator mode by " + nickname + "\r\n"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Mode::Mode() : ACommand("MODE") {}

Mode::~Mode() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Mode::executeCommand(Server *server) {
	int &fd = server->getFds()[server->getClientIndex()].fd;
	list<string> &tokens = server->getCommandHandler().getCommandTokens();
	list<string>::iterator it = tokens.begin();
	this->_channel = *it;
	clientInfo &user_info = server->getUserDB()[fd];
	this->_nickname = user_info._nickname;

	string error = parseFirstPart(server, tokens);
	if (!error.empty())
		return error;
	this->_mode = *++it;
	if ((this->_mode[0] == '-' || this->_mode[0] == '+') && (this->_mode[1] == 'i' || this->_mode[1] == 't' || this->_mode[1] == 'k' || this->_mode[1] == 'o' || this->_mode[1] == 'l')) {
		selectMode(server, it);
	} else
		return ERR_WRONGPARAMS(this->_nickname);
	return "";
}

string Mode::parseFirstPart(Server *server, const list<string> &tokens) {
	int &fd = server->getFds()[server->getClientIndex()].fd;
	clientInfo &user_info = server->getUserDB()[fd];

	if (!user_info._welcomed)
		return ERR_WELCOMED;
	if (tokens.size() < 2)
		return ERR_NEEDMOREPARAMS(this->_nickname);
	if (!server->getChannel(this->_channel))
		return ERR_NOSUCHCHANNEL(this->_channel);
	return "";
}

void Mode::selectMode(Server *server, list<string>::iterator it) {
	switch (this->_mode[1])
	{
	case 'i':
		modeI(server);
		break;
	case 't':
		modeT(server);
		break;
	case 'k':
		modeK(server, it);
		break;
	case 'o':
		modeO(server, it);
		break;
	case 'l':
		modeL(server, it);
		break;
	}
}

void Mode::modeI(Server *server) {
	if (this->_mode[0] == '+')
		server->getChannel(this->_channel)->setInviteRestrict(true);
	else
		server->getChannel(this->_channel)->setInviteRestrict(false);
}

void Mode::modeT(Server *server) {
	if (this->_mode[0] == '+')
		server->getChannel(this->_channel)->setTopicRestrict(true);
	else
		server->getChannel(this->_channel)->setTopicRestrict(false);
}

void Mode::modeK(Server *server, list<string>::iterator it) {
	Channel *channel = server->getChannel(this->_channel);
	if(it == server->getCommandHandler().getCommandTokens().end()) {    
		string error = ERR_NEEDMOREPARAMS(server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname);
		server->sendToClient(error);
		return ;
	}
	this->_mode_param = *++it;
	if(!isValidChar() && this->_mode_param.size() < 10) {
		string error = ERR_WRONGCHAR;
		server->sendToClient(error);
		return ;
	}
	if(_mode[0] == '+')
		channel->setPassword(this->_mode_param);
	else
		channel->setPassword("");
}

void Mode::modeO(Server *server, list<string>::iterator it) {
	Channel *channel = server->getChannel(this->_channel);
	if(it == server->getCommandHandler().getCommandTokens().end()) {
		string error = ERR_NEEDMOREPARAMS(server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname);
		server->sendToClient(error);
	}
	this->_mode_param = *++it;
	if(channel->getUserList()[server->getFds()[server->getClientIndex()].fd] != OPERATOR) {
		string error = ERR_CHANOPRIVSNEEDED(this->_nickname, this->_channel);
		server->sendToClient(error);
	} else if (_mode[0] == '+') {
		channel->getUserList()[findToOpFd(server)] = OPERATOR;
		string response = CLIENTOPTARGET(_nickname, _target);//mesage a formater correctement
		server->sendToClient(response);//mesage a formater correctement
		sendToUser(server);
		channel->broadcastListUser(server, server->getFds()[server->getClientIndex()].fd);

	} else if (_mode[0] == '-'){
		
	}
}

string Mode::sendToUser(Server *server){
	if(!server->isNickInServer(this->_target))
		return ERR_NOSUCHNICK(this->_target);
	string response = TARGETOPBYCLIENT(_nickname);
	send(findToOpFd(server), response.c_str(), response.size(), 0); //TODO gerer -1
	return "";
}

int Mode::findToOpFd(Server *server) {
	map<int, clientInfo> &user_db = server->getUserDB();
	for (map<int, clientInfo>::const_iterator it = user_db.begin(); it != user_db.end(); ++it)
		if (it->second._nickname == this->_nickname)
			return it->first;
	return 0;
}

void Mode::modeL(Server *server, list<string>::iterator it) {
	if(it == server->getCommandHandler().getCommandTokens().end()) {    
		string error = ERR_NEEDMOREPARAMS(server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname);
		server->sendToClient(error);
	}
	this->_mode_param = *++it;
}

bool Mode::isValidChar() {
	string characters = CHARACTERS_ALLOWED;

	if (this->_mode_param.find_first_not_of(characters) != string::npos)
			return false;
	return true;
}
