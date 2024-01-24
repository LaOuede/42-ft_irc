# include "Invite.hpp"
# include "Server.hpp"
# include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_WELCOMED "462 PRIVMSG :You are not authenticated\r\n"
#define ERR_NEEDMOREPARAMS(nickname) "461 PRVMSG " + nickname + " INVITE :Not enough parameters\r\n"
#define ERR_NOSUCHCHANNEL(channel) "403 " + channel + " :No such channel\r\n"
#define ERR_NOTONCHANNEL(nickname, channel) "442 " + nickname + " " + channel + " :You're not on that channel\r\n"
#define ERR_CHANOPRIVSNEEDED(nickname, channel) "482 " + nickname + " " + channel + " :You're not channel operator\r\n"
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
	string &nickname_invited = *tokens.begin();
	string &channel_token = *++tokens.begin();
	clientInfo &user_info = server->getUserDB()[fd];
	string &nickname = user_info._nickname;

	string error = parseFirstPart(server, tokens, channel_token);
	if (!error.empty())
		return error;

	Channel *channel = server->getChannel(channel_token);
	map<int, int> &user_list = channel->getUserList();
	if (user_list.find(fd) == user_list.end())
		return ERR_NOTONCHANNEL(nickname, channel_token);

	if (user_list[fd] != OPERATOR) //TODO ajuster quand il y aura des channels private avec les MOD?
		return ERR_CHANOPRIVSNEEDED(nickname, channel_token);
	int fd_invited = findClientToInvite(server, nickname_invited);
	if (fd_invited == fd)
		return ERR_CANTINVITESELF;
	else if (fd_invited == 0)
		return ERR_USERNOTEXIST(nickname_invited);

	channel->addUserToChannel(server, nickname_invited, fd_invited, USER);
	string message = RPL_INVITING(nickname, nickname_invited, channel_token);
	channel->broadcastToAll(message);
	return "";
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