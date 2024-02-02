#include "Mode.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_ALREADYMODE "400 :This mode is already set\r\n"
#define ERR_CHANOPRIVSNEEDED(nickname, channel) "482 " + nickname + " " + channel + " :You're not channel operator\r\n"
#define ERR_MODEYOURSELF "400 :You can't set/unset yourself operator\r\n"
#define ERR_NEEDMOREPARAMS(nickname) "461 " + nickname + " MODE :Not enough parameters\r\n"
#define ERR_NOSUCHCHANNEL(channel) "403 " + channel + " :No such channel\r\n"
#define ERR_NOSUCHNICK(mode_param) "401 " + mode_param + " :No such nick\r\n"
#define ERR_NOTONCHANNEL(nickname, channel) "442 " + nickname + " " + channel + " :You're not on that channel\r\n"
#define ERR_TOOMANYPARAMS(function) "400 " + function + " :Too many parameters\r\n"
#define ERR_UMODEUNKNOWNFLAG(nickname) "501 " + nickname + " :Unknown MODE flag\r\n"
#define ERR_WELCOMED "462 PRIVMSG :You are not authenticated\r\n"
#define ERR_WRONGCHAR "400 :Wrong character or password too long\r\n"
#define ERR_WRONGLIMIT(nickname) "461 " + nickname + " MODE :Syntax error OR out of range size\r\n"
#define ERR_WRONGPARAMS(nickname) "461 " + nickname + " MODE :Syntax error\r\n"
#define RPL_CLIENTOPTARGET(nickname, channel, mode_param) ":" + nickname + " MODE " + channel + " :" + mode_param + " is promoted operator\r\n"
#define RPL_CLIENTDEOPTARGET(nickname, channel, mode_param) ":" + nickname + " MODE " + channel + " :" + mode_param + " is demoted from operator\r\n"
#define RPL_INVITEON(nickname, channel) ":" + nickname + " MODE " + channel + " :Invitation IS requested\r\n"
#define RPL_INVITEOFF(nickname, channel) ":" + nickname + " MODE " + channel + " :Invitation NOT requested\r\n"
#define RPL_LIMITON(nickname, channel, users_limit) ":" + nickname + " MODE " + channel + " :" + users_limit + " is the new users limit\r\n"
#define RPL_LIMITOFF(nickname, channel) ":" + nickname + " MODE " + channel + " :No more user limit\r\n"
#define RPL_PASSWORDON(nickname, channel) ":" + nickname + " MODE " + channel + " :Password IS requested\r\n"
#define RPL_PASSWORDOFF(nickname, channel) ":" + nickname + " MODE " + channel + " :Password NOT requested\r\n"
#define RPL_TOPICON(nickname, channel) ":" + nickname + " MODE " + channel + " :Topic IS restricted\r\n"
#define RPL_TOPICOFF(nickname, channel) ":" + nickname + " MODE " + channel + " :Topic NOT restricted\r\n"


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
	_channel = *it;
	clientInfo &user_info = server->getUserDB()[fd];
	_nickname = user_info._nickname;

	string error = parseFirstPart(server, tokens);
	if (!error.empty()) {
		return error;
	}
	if (tokens.size() == 1) {
		Channel *channel = server->getChannel(_channel);
		channel->broadcastChannelMode(server, _nickname);
		return "";
	}
	_mode = *++it;
	if ((_mode[0] == '-' || _mode[0] == '+')
		&& (_mode[1] == 'i' || _mode[1] == 't' || _mode[1] == 'k'|| _mode[1] == 'o' || _mode[1] == 'l') && _mode.size() == 2) {
		selectMode(server, it);
	} else {
		return ERR_UMODEUNKNOWNFLAG(_nickname);
	}
	return "";
}

string Mode::parseFirstPart(Server *server, const list<string> &tokens) {
	int &fd = server->getFds()[server->getClientIndex()].fd;
	clientInfo &user_info = server->getUserDB()[fd];

	if (!user_info._welcomed)
		return ERR_WELCOMED;
	if (tokens.size() < 1)
		return ERR_NEEDMOREPARAMS(_nickname);
	if (!server->isChannelInServer(_channel))
		return ERR_NOSUCHCHANNEL(_channel);
	return "";
}

void Mode::selectMode(Server *server, list<string>::iterator it) {
	switch (_mode[1])
	{
	case 'i':
		modeInvite(server);
		break;
	case 't':
		modeTopic(server);
		break;
	case 'k':
		modePassword(server, it);
		break;
	case 'o':
		modeOperator(server, it);
		break;
	case 'l':
		modeLimit(server, it);
		break;
	}
}

void Mode::modeInvite(Server *server) {
	if (server->getCommandHandler().getCommandTokens().size() > 2) {
		server->sendToClient(ERR_WRONGPARAMS(_nickname));
		return;
	}

	Channel *channel = server->getChannel(_channel);
	if (checkUserStatus(server, channel)) {
		if (_mode[0] == '+') {
			channel->setInviteRestrict(true);
			channel->broadcastToAll(RPL_INVITEON(_nickname, _channel));
		} else {
			channel->setInviteRestrict(false);
			channel->broadcastToAll(RPL_INVITEOFF(_nickname, _channel));
		}
	}
}

void Mode::modeTopic(Server *server) {
	if (server->getCommandHandler().getCommandTokens().size() > 2) {
		server->sendToClient(ERR_WRONGPARAMS(_nickname));
		return;
	}

	Channel *channel = server->getChannel(_channel);
	if (checkUserStatus(server, channel)) {
		if (_mode[0] == '+') {
			channel->setTopicRestrict(true);
			channel->broadcastToAll(RPL_TOPICON(_nickname, _channel));
		} else {
			channel->setTopicRestrict(false);
			channel->broadcastToAll(RPL_TOPICOFF(_nickname, _channel));
		}
	}
}

void Mode::modePassword(Server *server, list<string>::iterator it) {
	Channel *channel = server->getChannel(_channel);
	if (it == server->getCommandHandler().getCommandTokens().end()) {	
		server->sendToClient(ERR_NEEDMOREPARAMS(server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname));
		return ;
	}
	if (checkUserStatus(server, channel)) {
		if (server->getCommandHandler().getCommandTokens().size() == 3 && _mode[0] == '+') {
			_mode_param = *++it;
			if (!isValidChar() || _mode_param.size() > 10) {
				server->sendToClient(ERR_WRONGCHAR);
				return ;
			}
			channel->setPassword(_mode_param);
			channel->broadcastToAll(RPL_PASSWORDON(_nickname, _channel));
		} else if (server->getCommandHandler().getCommandTokens().size() == 2 && _mode[0] == '-') {
			channel->setPassword("");
			channel->broadcastToAll(RPL_PASSWORDOFF(_nickname, _channel));
		} else {
			server->sendToClient(ERR_WRONGPARAMS(_nickname));
		}
	}
}

bool Mode::isValidChar() {
	if (_mode_param.find_first_not_of(CHARACTERS_ALLOWED) != string::npos)
			return false;
	return true;
}

void Mode::modeOperator(Server *server, list<string>::iterator it) {
	Channel *channel = server->getChannel(_channel);
	if (checkUserStatus(server, channel)){
		string msg = parseOpParameter(server, it, channel);
		if(!msg.empty()){
			server->sendToClient(msg);
			return ;
		}
		if (_mode[0] == '+') {
			if (changeUserMode(server, OPERATOR))
				return ;
			msg = RPL_CLIENTOPTARGET(_nickname, _channel, _mode_param);
		} else if (_mode[0] == '-'){
			if (changeUserMode(server, USER))
				return;
			msg = RPL_CLIENTDEOPTARGET(_nickname, _channel, _mode_param);
		}
			channel->broadcastToAll(msg);
			channel->broadcastListUser(server, server->getFds()[server->getClientIndex()].fd);
	}
}

string Mode::parseOpParameter(Server *server, list<string>::iterator it, Channel *channel) {
	if (it == server->getCommandHandler().getCommandTokens().end())
		return ERR_NEEDMOREPARAMS(_nickname);
	_mode_param = *++it;
	if(!server->isNickInServer(_mode_param) || !channel->isUserInChannel(findToOpFd(server)))
		return (ERR_NOSUCHNICK(_mode_param));
	if (_nickname == _mode_param)
		return (ERR_MODEYOURSELF);
	return "";
}

int Mode::changeUserMode(Server *server, int mode){
	map<int, int> &userlist = server->getChannel(_channel)->getUserList();
	map<int, int>::iterator it = userlist.begin();
	for(; it != userlist.end(); it++){
		if(it->first == findToOpFd(server)){
			if(it->second != mode)
				it->second = mode;
			else{
				server->sendToClient(ERR_ALREADYMODE);
				return 1;
			}
		}
	}
	return 0;
}

int Mode::findToOpFd(Server *server) {
	map<int, clientInfo> &user_db = server->getUserDB();
	for (map<int, clientInfo>::const_iterator it = user_db.begin(); it != user_db.end(); ++it)
		if (it->second._nickname == _mode_param)
			return it->first;
	return 0;
}

void Mode::modeLimit(Server *server, list<string>::iterator it) {
	if (server->getCommandHandler().getCommandTokens().size() > 3) {
		server->sendToClient(ERR_TOOMANYPARAMS(server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname));
		return ;
	}

	Channel *channel = server->getChannel(_channel);
	if (checkUserStatus(server, channel)) {
		if (server->getCommandHandler().getCommandTokens().size() == 3 && _mode[0] == '+') {
			modeLimitON(server, channel, it);
		} else if (server->getCommandHandler().getCommandTokens().size() == 2 && _mode[0] == '-') {
			modeLimitOFF(channel);
		} else {
			server->sendToClient(ERR_WRONGLIMIT(_nickname));
		}
	}
}

bool Mode::checkUserStatus(Server *server, Channel *channel) {
	int &fd = server->getFds()[server->getClientIndex()].fd;

	if (channel->getUserList().find(fd) == channel->getUserList().end()) {
		server->sendToClient(ERR_NOTONCHANNEL(_nickname, _channel));
		return false;
	}
	if(channel->getUserList()[server->getFds()[server->getClientIndex()].fd] != OPERATOR) {
		server->sendToClient(ERR_CHANOPRIVSNEEDED(_nickname, _channel));
		return false;
	}
	return true;
}

bool Mode::isValidNumber(string &number) {
	for (size_t i = 0; i < number.size(); i++) {
		if (!isdigit(number[i])) {
			return false;
		}
	}
	if (atoi(number.c_str()) < 1 || atoi(number.c_str()) > MAXINCHANNEL) {
		return false;
	}
	return true;
}

void Mode::modeLimitON(Server *server, Channel *channel, list<string>::iterator it) {
	_users_limit = *++it;
	if (isValidNumber(_users_limit)) {
		channel->setLimitRestrict(true);
		channel->setUsersLimit(atoi(_users_limit.c_str()));
		channel->broadcastToAll(RPL_LIMITON(_nickname, _channel, _users_limit));
	} else {
		server->sendToClient(ERR_WRONGPARAMS(_nickname));
	}
}

void Mode::modeLimitOFF(Channel *channel) {
	channel->setLimitRestrict(false);
	channel->setUsersLimit(-1);
	channel->broadcastToAll(RPL_LIMITOFF(_nickname, _channel));
}
