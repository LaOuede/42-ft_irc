#include "Invite.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_WELCOMED "462 PRIVMSG :You are not authenticated\r\n"
#define ERR_NEEDMOREPARAMS(nickname) "461 PRVMSG " + nickname + " INVITE :Not enough parameters\r\n"
#define ERR_NOSUCHCHANNEL(channel) "403 " + channel + " :No such channel\r\n"
#define ERR_NOTONCHANNEL(nickname, channel) "442 " + nickname + " " + channel + " :You're not on that channel\r\n"
#define ERR_CHANOPRIVSNEEDED(nickname, channel) "482 " + nickname + " " + channel + " :You're not channel operator\r\n"
#define ERR_CHANOPRIVSNEEDED2(nickname, channel) "482 " + nickname + " " + channel + " :Channel is not in invite mode\r\n"
#define ERR_USERNOTEXIST(user) "401 " + user + " :No such user in the database\r\n"
#define ERR_CANTINVITESELF "437 :You can't invite yourself\r\n"
#define RPL_INVITING(nickname, nickname_invited, channel) ":" + nickname + " INVITE " + nickname_invited + " " + channel + "\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Invite::Invite() : ACommand("INVITE") {}

Invite::~Invite() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Invite::executeCommand(Server *server) {
	int	&fd = server->getFds()[server->getClientIndex()].fd;
	list<string> &tokens = server->getCommandHandler().getCommandTokens();
	clientInfo &user_info = server->getUserDB()[fd];
	_nickname = user_info._nickname;

	if (!authentificationCheck(server)) {
		return ERR_WELCOMED;
	}
	if (tokens.size() < 2)
		return ERR_NEEDMOREPARAMS(_nickname);
	_invited = *tokens.begin();
	_channel = *++tokens.begin();
	string error = parseFirstPart(server, _channel);
	if (!error.empty())
		return error;

	Channel *channel = server->getChannel(_channel);
	if (!channel->getInviteRestrict())
		return ERR_CHANOPRIVSNEEDED2(_nickname, _channel);
	map<int, int> &user_list = channel->getUserList();
	if (user_list.find(fd) == user_list.end())
		return ERR_NOTONCHANNEL(_nickname, _channel);

	if ((isClientInvited(server, fd, _channel) && user_list[fd] != OPERATOR) || !isClientInvited(server, fd, _channel))
		return ERR_CHANOPRIVSNEEDED(_nickname, _channel);
	_fd_invited = findClientToInvite(server, _invited);
	if (_fd_invited == fd)
		return ERR_CANTINVITESELF;
	else if (_fd_invited == 0)
		return ERR_USERNOTEXIST(_invited);

	list<int> &invited = server->getChannel(_channel)->getGuestsList();
	invited.push_back(_fd_invited);
	string message = RPL_INVITING(_nickname, _invited, _channel);
	server->sendToClient(message);
	if (send(_fd_invited, message.c_str(), message.size(), 0) == -1)
		std::cerr << "Error : SEND return -1" << endl;
	return "";
}

bool Invite::authentificationCheck(Server *server) {
	int &fd = server->getFds()[server->getClientIndex()].fd;
	return (server->getUserDB()[fd]._welcomed == false) ? false : true;
}

bool Invite::isClientInvited(Server *server, int fd, string channel_token) {
	list<int> &invited = server->getChannel(channel_token)->getGuestsList();
	for (list<int>::const_iterator it = invited.begin(); it != invited.end(); ++it) {
		if (*it == fd)
			return true;
	}
	return false;
}

int Invite::findClientToInvite(Server *server, const string &nickname_invited) {
	map<int, clientInfo> &user_db = server->getUserDB();
	for (map<int, clientInfo>::const_iterator it = user_db.begin(); it != user_db.end(); ++it) {
		if (it->second._nickname == nickname_invited) {
			return it->first;
		}
	}
	return 0;
}

string Invite::parseFirstPart(Server *server, const string &channel_token) {
	if (!server->isChannelInServer(_channel))
		return ERR_NOSUCHCHANNEL(channel_token);
	return "";
}
