# include "Kick.hpp"
# include "Server.hpp"
# include "CommandHandler.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_NEEDMOREPARAMS "461 USER :Not enough parameters\r\n"
#define ERR_NOTONCHANNEL(nickname, channel) "442 " + nickname + " " + channel + " :You're not on that channel\r\n"
#define ERR_WELCOMED "462 PRIVMSG :You are not authenticated\r\n"
#define ERR_NOSUCHCHANNEL(channel) "403 " + channel + " :No such channel\r\n"
#define ERR_USERNOTINCHANNEL(nickname, channel) "441 " + nickname + " " + channel + " :They aren't on that channel\r\n"
#define ERR_CHANOPRIVSNEEDED(nickname, channel) "482 " + nickname + " " + channel + " :You're not channel operator\r\n"
#define ERR_USERNOTEXIST(user) "401 " + user + " :No such user in the database\r\n"
#define ERR_CANTKICKSELF "437 :You can't kick yourself\r\n"
#define KICK(nickname, hostname, user, channel, comment) ":" + nickname + "@" + hostname + " KICK " + user + " from channel " + channel + " " + comment + "\r\n"
#define ERR_WRONGCHAR4 "400 :Supposed to be : at the beginning of the comment\r\n"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Kick::Kick() : ACommand("KICK") {}

Kick::~Kick() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Kick::executeCommand(Server *server) {
	string &hostname = server->getHostname();
	int	&fd = server->getFds()[server->getClientIndex()].fd;
	list<string> &tokens = server->getCommandHandler().getCommandTokens();
	string &channel_token = *tokens.begin();
	clientInfo &user_info = server->getUserDB()[fd];
	string &nickname = user_info._nickname;

	if (user_info._welcomed == false)
		return (ERR_WELCOMED);
	if (tokens.size() < 2)
		return (ERR_NEEDMOREPARAMS);
	if (!server->getChannel(channel_token))
		return (ERR_NOSUCHCHANNEL(channel_token));
	Channel &channels = *server->getChannel(channel_token);
	map<int, int> &user_list = channels.getUserList();
	if (!user_list.find(fd)->first)
		return (ERR_NOTONCHANNEL(nickname, channel_token));
	if (user_list.find(fd)->second != OPERATOR)
		return (ERR_CHANOPRIVSNEEDED(nickname, channel_token));
	string &user_kicked = *++tokens.begin();
	map<int, clientInfo> &user_db = server->getUserDB();
	map<int, clientInfo>::iterator it = user_db.begin();
	int fd_kicked = 0;
	for (; it != user_db.end(); it++) {
		if (it->second._username == user_kicked)
			fd_kicked = it->first;
	}
	if (fd_kicked == fd)
		return (ERR_CANTKICKSELF);
	else if (fd_kicked == 0)
		return (ERR_USERNOTEXIST(user_kicked));
	if (!user_list.find(fd_kicked)->first)
		return (ERR_USERNOTINCHANNEL(user_kicked, channel_token));
	list<string>::iterator it2 = ++tokens.begin();
	string comment = *++it2;
	if (tokens.size() == 2) {
		comment = ":No comment";
		it2 = --tokens.end();
		cout << "comment: " << comment << endl;
	}
	else if (comment[0] != ':')
		return (ERR_WRONGCHAR4);
	string old_comment;
	while (++it2 != tokens.end()) {
		old_comment = comment;
		comment = old_comment + " " + *it2;
	}
	user_list.erase(fd_kicked);
	return (KICK(nickname, hostname, user_kicked, channel_token, comment));
}
