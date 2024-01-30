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

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Mode::Mode() : ACommand("MODE") {}

Mode::~Mode() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Mode::executeCommand(Server *server) {
	int	&fd = server->getFds()[server->getClientIndex()].fd;
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
	int	&fd = server->getFds()[server->getClientIndex()].fd;
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

void Mode::modeOperator(Server *server, list<string>::iterator it) {
	Channel *channel = server->getChannel(_channel);
	if (it == server->getCommandHandler().getCommandTokens().end()) {	
		server->sendToClient(ERR_NEEDMOREPARAMS(server->getUserDB()[server->getFds()[server->getClientIndex()].fd]._nickname));
		channel->broadcastToAll("400 MODE :Operator Restrict mode is ON\r\n");
	}
	_mode_param = *++it;
	if(channel->getUserList()[server->getFds()[server->getClientIndex()].fd] != OPERATOR) {
		server->sendToClient(ERR_CHANOPRIVSNEEDED(_nickname, _channel));
		channel->broadcastToAll("400 MODE :Operator Restrict mode is OFF\r\n");
	}
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