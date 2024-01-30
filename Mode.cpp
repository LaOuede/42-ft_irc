#include "Mode.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_NEEDMOREPARAMS(nickname) "461 " + nickname + " MODE :Not enough parameters\r\n"
#define ERR_WELCOMED "462 MODE :You are not authenticated\r\n"
#define ERR_NOTONCHANNEL(nickname, channel) "442 " + nickname + " " + channel + " :You're not on that channel\r\n"
#define ERR_NOSUCHCHANNEL(channel) "403 " + channel + " :No such channel\r\n"
#define ERR_PASSWDMISMATCH "464 PRIVMSG :Password incorrect\r\n"
#define ERR_WRONGCHAR "400 :Wrong character or too long password\r\n"
#define ERR_CHANOPRIVSNEEDED(nickname, channel) "482 " + nickname + " " + channel + " :You're not channel operator\r\n"
#define ERR_WRONGPARAMS(nickname) "461 " + nickname + " MODE :Syntax error\r\n"
#define ERR_NOSUCHNICK(target) "401 " + target + " :No such nick\r\n"
#define ERR_ALREADYMODE "400 :This mode is already set\r\n"
#define ERR_MODEYOURSELF "400 :You can't set/unset operator yourself\r\n"
#define CLIENTOPTARGET(nickname, channel, mode_param) ":" + nickname + " MODE " + channel + " :" + mode_param + " is promote operator \r\n"
#define CLIENTDEOPTARGET(nickname, channel, mode_param) ":" + nickname + " MODE " + channel + " :" + mode_param + " is demote from operator \r\n"

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
	_mode = *++it;
	if ((_mode[0] == '-' || _mode[0] == '+')
		&& (_mode[1] == 'i' || _mode[1] == 't' || _mode[1] == 'k'|| _mode[1] == 'o' || _mode[1] == 'l')) {
		selectMode(server, it);
	} else {
		return ERR_WRONGPARAMS(_nickname);
	}
	return "";
}

string Mode::parseFirstPart(Server *server, const list<string> &tokens) {
	int &fd = server->getFds()[server->getClientIndex()].fd;
	clientInfo &user_info = server->getUserDB()[fd];

	if (!user_info._welcomed)
		return ERR_WELCOMED;
	if (tokens.size() < 2)
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
	if (_mode[0] == '+') {
		channel->setInviteRestrict(true);
		channel->broadcastToAll("400 MODE :Invite Restrict mode is ON\r\n");
	} else {
		channel->setInviteRestrict(false);
		channel->broadcastToAll("400 MODE :Invite Restrict mode is OFF\r\n");
	}
}

void Mode::modeTopic(Server *server) {
	if (server->getCommandHandler().getCommandTokens().size() > 2) {
		server->sendToClient(ERR_WRONGPARAMS(_nickname));
		return;
	}

	Channel *channel = server->getChannel(_channel);
	if (_mode[0] == '+') {
		channel->setTopicRestrict(true);
		channel->broadcastToAll("400 MODE :Topic Restrict mode is ON\r\n");
	} else {
		channel->setTopicRestrict(false);
		channel->broadcastToAll("400 MODE :Topic Restrict mode is OFF\r\n");
	}
}

void Mode::modePassword(Server *server, list<string>::iterator it) {
	Channel *channel = server->getChannel(_channel);
	if (it == server->getCommandHandler().getCommandTokens().end()) {	
		server->sendToClient(ERR_NEEDMOREPARAMS(server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname));
		return ;
	}
	if (!isValidChar() && _mode_param.size() < 10) {
		server->sendToClient(ERR_WRONGCHAR);
		return ;
	}
	if (server->getCommandHandler().getCommandTokens().size() == 3 && _mode[0] == '+') {
		_mode_param = *++it;
		channel->setPassword(_mode_param);
		channel->broadcastToAll("400 MODE :Password Restrict mode is ON\r\n");
	} else if (server->getCommandHandler().getCommandTokens().size() == 2 && _mode[0] == '-') {
		channel->setPassword("");
		channel->broadcastToAll("400 MODE :Password Restrict mode is OFF\r\n");
	} else {
		server->sendToClient(ERR_WRONGPARAMS(_nickname));
	}
}

void Mode::modeO(Server *server, list<string>::iterator it) {
	Channel *channel = server->getChannel(_channel);
	string msg;
	if (it == server->getCommandHandler().getCommandTokens().end()) {
		string error = ERR_NEEDMOREPARAMS(server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname);
		server->sendToClient(error);
	}
	_mode_param = *++it;
	if (_nickname == _mode_param){
		string error = ERR_MODEYOURSELF;
		server->sendToClient(error);
		return ;
	}
	if (channel->getUserList()[server->getFds()[server->getClientIndex()].fd] != OPERATOR) {
		string error = ERR_CHANOPRIVSNEEDED(_nickname, _channel);
		server->sendToClient(error);
	} else if (_mode[0] == '+') {
		if (changeUserMode(server, OPERATOR))
			return ;
		msg = CLIENTOPTARGET(_nickname, _channel, _mode_param);

	} else if (_mode[0] == '-'){
		if (changeUserMode(server, USER))
			return;
		msg = CLIENTDEOPTARGET(_nickname, _channel, _mode_param);
	}
		channel->broadcastToAll(msg);
		channel->broadcastListUser(server, server->getFds()[server->getClientIndex()].fd);
}

int Mode::changeUserMode(Server *server, int mode){
	map<int, int> &userlist = server->getChannel(_channel)->getUserList();
	map<int, int>::iterator it = userlist.begin();
	for(; it != userlist.end(); it++){
		if(it->first == findToOpFd(server)){
			if(it->second != mode)
				it->second = mode;
			else{
				string response = ERR_ALREADYMODE;
				server->sendToClient(response);
				return 1;
			}
		}
	}
	return 0;
}

// string Mode::sendToUser(Server *server){
// 	if(!server->isNickInServer(_mode_param))
// 		return ERR_NOSUCHNICK(_mode_param);
// 	string response = TARGETOPBYCLIENT(_nickname);
// 	send(findToOpFd(server), response.c_str(), response.size(), 0); //TODO gerer -1
// 	return "";
// }

int Mode::findToOpFd(Server *server) {
	map<int, clientInfo> &user_db = server->getUserDB();
	for (map<int, clientInfo>::const_iterator it = user_db.begin(); it != user_db.end(); ++it)
		if (it->second._nickname == _mode_param)
			return it->first;
	return 0;
}

void Mode::modeLimit(Server *server, list<string>::iterator it) {
	if (server->getCommandHandler().getCommandTokens().size() > 3) {
		server->sendToClient(ERR_NEEDMOREPARAMS(server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname));
		return ;
	}

	Channel *channel = server->getChannel(_channel);
	int &fd = server->getFds()[server->getClientIndex()].fd;
	if (channel->getUserList().find(fd) == channel->getUserList().end()) {
		server->sendToClient(ERR_NOTONCHANNEL(_nickname, _channel));
		return;
	}
	if(channel->getUserList()[server->getFds()[server->getClientIndex()].fd] != OPERATOR) {
		server->sendToClient(ERR_CHANOPRIVSNEEDED(_nickname, _channel));
		return;
	}

	if (server->getCommandHandler().getCommandTokens().size() == 3 && _mode[0] == '+') {
		_users_limit = *++it;
		if (isValidNumber(_users_limit)) {
			channel->setLimitRestrict(true);
			channel->setUsersLimit(atoi(_users_limit.c_str()));
			channel->broadcastToAll("400 MODE :Limit Restrict mode is ON\r\n");
		} else {
			server->sendToClient(ERR_WRONGPARAMS(_nickname));
			return;
		}
	} else if (server->getCommandHandler().getCommandTokens().size() == 2 && _mode[0] == '-') {
		channel->setLimitRestrict(false);
		channel->setUsersLimit(-1);
		channel->broadcastToAll("400 MODE :Limit Restrict mode is OFF\r\n");
	} else {
		server->sendToClient(ERR_WRONGPARAMS(_nickname));
	}

}

bool Mode::isValidChar() {
	string characters = CHARACTERS_ALLOWED;

	if (_mode_param.find_first_not_of(characters) != string::npos)
			return false;
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