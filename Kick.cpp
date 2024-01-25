#include "Kick.hpp"
#include "Server.hpp"
#include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_NEEDMOREPARAMS(nickname) "461 PRVMSG " + nickname + " KICK :Not enough parameters\r\n"
#define ERR_NOTONCHANNEL(nickname, channel) "442 " + nickname + " " + channel + " :You're not on that channel\r\n"
#define ERR_WELCOMED "462 PRIVMSG :You are not authenticated\r\n"
#define ERR_NOSUCHCHANNEL(channel) "403 " + channel + " :No such channel\r\n"
#define ERR_USERNOTINCHANNEL(nickname, channel) "441 " + nickname + " " + channel + " :They aren't on that channel\r\n"
#define ERR_CHANOPRIVSNEEDED(nickname, channel) "482 " + nickname + " " + channel + " :You're not channel operator\r\n"
#define ERR_USERNOTEXIST(user) "401 " + user + " :No such user in the database\r\n"
#define ERR_CANTKICKSELF "437 :You can't kick yourself\r\n"
#define KICK(nickname, channel, nickname_kicked, comment) ":" + nickname + " KICK " + channel + " " + nickname_kicked + " " + comment + "\r\n"
#define ERR_WRONGCHAR4 "400 :Supposed to be : at the beginning of the comment\r\n"
#define RPL_QUITCHANNEL(user, channel) ":" + user + " PART " + channel + "\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Kick::Kick() : ACommand("KICK") {}

Kick::~Kick() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Kick::executeCommand(Server *server) {
	int	&fd = server->getFds()[server->getClientIndex()].fd;
	list<string> &tokens = server->getCommandHandler().getCommandTokens();
	string &channel_token = *tokens.begin();
	clientInfo &user_info = server->getUserDB()[fd];
	string &nickname = user_info._nickname;

	string error = parseFirstPart(server, tokens, channel_token);
	if (!error.empty())
		return error;

	Channel *channel = server->getChannel(channel_token);
	map<int, int> &user_list = channel->getUserList();
	if (user_list.find(fd) == user_list.end())
		return ERR_NOTONCHANNEL(nickname, channel_token);

	if (user_list[fd] != OPERATOR)
		return ERR_CHANOPRIVSNEEDED(nickname, channel_token);

	string &nickname_kicked = *++tokens.begin();
	int fd_kicked = findClientToKick(server, nickname_kicked);
	if (fd_kicked == fd)
		return ERR_CANTKICKSELF;
	else if (fd_kicked == 0)
		return ERR_USERNOTEXIST(nickname_kicked);

	if (user_list.find(fd_kicked) == user_list.end())
		return ERR_USERNOTINCHANNEL(nickname_kicked, channel_token);
		
	string comment = getComment(tokens);
	string message = KICK(nickname, channel_token, nickname_kicked, comment);
	channel->broadcastToAll(message);
	string &nick_kicked = server->getUserDB()[fd_kicked]._nickname;
	broadcastUserQuitMessage(channel, nick_kicked);
	channel->removeUserFromChannel(server, fd_kicked);
	return "";
}

void Kick::broadcastUserQuitMessage(Channel *channel, const string &user) {
	const string &channel_name = channel->getChannelName();
	string msg = RPL_QUITCHANNEL(user, channel_name);
	channel->broadcastToAll(msg);
}

string Kick::parseFirstPart(Server *server, const list<string> &tokens, const string &channel_token) {
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

int Kick::findClientToKick(Server *server, const string &nickname_kicked) {
	map<int, clientInfo> &user_db = server->getUserDB();
	for (map<int, clientInfo>::const_iterator it = user_db.begin(); it != user_db.end(); ++it) {
		if (it->second._nickname == nickname_kicked) {
			return it->first;
		}
	}
	return 0;
}

string Kick::getComment(const list<string> &tokens) {
	list<string>::const_iterator it2 = ++tokens.begin();

	if (tokens.size() == 2) {
		string comment = ":No comment";
		return comment;
	}
	string comment = *++it2;
	while (++it2 != tokens.end()) {
		comment += " " + *it2;
		if (comment.length() > 50)
			break;
	}
	return comment;
}
