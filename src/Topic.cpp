#include "Topic.hpp"
#include "Server.hpp"

/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define ERR_CHANOPRIVSNEEDED(nickname, channel) "482 " + nickname + " " + channel + " :You're not channel operator\r\n"
#define ERR_NEEDMOREPARAMS(nickname) "461 PRVMSG " + nickname + " TOPIC :Not enough parameters\r\n"
#define ERR_NOSUCHCHANNEL(channel) "403 " + channel + " :No such channel\r\n"
#define ERR_NOTONCHANNEL(nickname, channel) "442 " + nickname + " " + channel + " :You're not on that channel\r\n"
#define ERR_WELCOMED "462 PRIVMSG :You are not authenticated\r\n"
#define RPL_TOPIC(nickname, channel, topic) "332 " + nickname + " " + channel + " :" + topic + "\r\n"
#define RPL_NOTOPIC(nickname, channel) "331 " + nickname + " " + channel + " :No topic is set\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Topic::Topic() : ACommand("TOPIC"){}

Topic::~Topic() {}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
string Topic::executeCommand(Server *server) {
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

	if (channel->getTopicRestrict() && user_list[fd] != OPERATOR)
		return ERR_CHANOPRIVSNEEDED(nickname, channel_token);

	if ((channel->getTopicRestrict() && user_list[fd] == OPERATOR) || !channel->getTopicRestrict()) {
		_topic = findTopic(server, tokens, channel);
		channel->setTopic(_topic);
		if (_topic.empty())
			return "";
		string topic_message = RPL_TOPIC(nickname, channel_token, _topic);
		channel->broadcastToAll(topic_message);
	}
	return "";
}

string Topic::parseFirstPart(Server *server, const list<string> &tokens, const string &channel_token) {
	int	&fd = server->getFds()[server->getClientIndex()].fd;
	clientInfo &user_info = server->getUserDB()[fd];
	string &nickname = user_info._nickname;

	if (!user_info._welcomed)
		return ERR_WELCOMED;
	if (tokens.size() < 1)
		return ERR_NEEDMOREPARAMS(nickname);
	if (!server->isChannelInServer(channel_token))
		return ERR_NOSUCHCHANNEL(channel_token);
	return "";
}

string Topic::findTopic(Server *server, const list<string> &tokens, Channel *channel) {
	int	&fd = server->getFds()[server->getClientIndex()].fd;
	clientInfo &user_info = server->getUserDB()[fd];
	string &nickname = user_info._nickname;
	list<string>::const_iterator it2 = ++tokens.begin();
	string topic;
	if (tokens.size() == 1) {
		if (channel->getTopic().empty()) {
			string topic_message = RPL_NOTOPIC(nickname, channel->getChannelName());
			channel->broadcastToAll(topic_message);
			return "";
		}
		else
			return channel->getTopic();
	} else if (tokens.size() == 2 && (*it2)[0] == ':' && (*it2)[1] == ':'){
		channel->setTopic("");
		string topic_message = RPL_NOTOPIC(nickname, channel->getChannelName());
		channel->broadcastToAll(topic_message);
		return "";
	} else {
		topic = *it2;
		topic.erase(0, 1);
		while (++it2 != tokens.end()) {
			topic += " " + *it2;
			if (topic.length() > 25)
				break;
		}
	}
	return topic;
}
