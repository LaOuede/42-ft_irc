#ifndef TOPIC_HPP
#define TOPIC_HPP

#pragma once

#include "ACommand.hpp"
#include <cstring>
#include <iostream>
#include <list>

using std::cout;
using std::endl;
using std::list;
using std::string;

class Server;

class Channel;

class Topic : public ACommand {
	public:
		// Construtors & Destructors
		Topic();
		virtual	~Topic();

		// Methods
		string	executeCommand(Server *server);
		string	parseFirstPart(Server *server, const list<string> &tokens, const string &channel_token);
		string	findTopic(Server *server, const list<string> &tokens, Channel *channel);

	private:
		// Attributes
		string	_name;
		string	_topic;
};

#endif