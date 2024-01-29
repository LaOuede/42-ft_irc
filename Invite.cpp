#include "Invite.hpp"
#include "Server.hpp"
#include "CommandHandler.hpp"

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
	this->_invited = *tokens.begin();
	this->_channel = *++tokens.begin();
	clientInfo &user_info = server->getUserDB()[fd];
	this->_nickname = user_info._nickname;

	string error = parseFirstPart(server, tokens, this->_channel);
	if (!error.empty())
		return error;

	Channel *channel = server->getChannel(this->_channel);
	if (!channel->getInviteRestrict())
		return ERR_CHANOPRIVSNEEDED2(this->_nickname, this->_channel);
	map<int, int> &user_list = channel->getUserList();
	if (user_list.find(fd) == user_list.end())
		return ERR_NOTONCHANNEL(this->_nickname, this->_channel);

	if ((isClientInvited(server, fd, this->_channel) && user_list[fd] != OPERATOR) || !isClientInvited(server, fd, this->_channel))
		return ERR_CHANOPRIVSNEEDED(this->_nickname, this->_channel);
	this->_fd_invited = findClientToInvite(server, this->_invited);
	if (this->_fd_invited == fd)
		return ERR_CANTINVITESELF;
	else if (this->_fd_invited == 0)
		return ERR_USERNOTEXIST(this->_invited);

	list<int> &invited = server->getChannel(this->_channel)->getGuestsList();
	invited.push_back(this->_fd_invited);
	string message = RPL_INVITING(this->_nickname, this->_invited, this->_channel);
	server->sendToClient(message);
	send(this->_fd_invited, message.c_str(), message.size(), 0);
	return "";
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

string Invite::parseFirstPart(Server *server, const list<string> &tokens, const string &channel_token) {
	int	&fd = server->getFds()[server->getClientIndex()].fd;
	clientInfo &user_info = server->getUserDB()[fd];
	string &nickname = user_info._nickname;

	if (!user_info._welcomed)
		return ERR_WELCOMED;
	if (tokens.size() < 2)
		return ERR_NEEDMOREPARAMS(nickname);
	if (!server->getChannel(channel_token))
		return ERR_NOSUCHCHANNEL(channel_token);
	return "";
}
