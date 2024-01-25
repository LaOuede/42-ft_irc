#ifndef JOIN_HPP
#define JOIN_HPP

#pragma once

#include "ACommand.hpp"
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <vector>

#define CHANLIMIT 5

using std::cout;
using std::endl;
using std::list;
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

class Server;

class Join : public ACommand {
	public:
		// Construtors & Destructors
		Join();
		virtual ~Join();

		// Methods
		string							executeCommand(Server *server);
		string							parseCommand(Server *server);
		string							parseParameters(const list<string> &command);
		void							splitParameters(string const &to_split, list<string> &to_fill);
		string							parseAttributes(const list<string> &command);
		void							createChannelVector();
		string							processChannelConnections(Server *Server);
		string							parseChannelNameAndKey(string const &name, string key);
		void							joinChannel(Server *server, string const &channel_name);
		bool							isChannelExisting(Server *server, string const &channel_name);
		void							createChannel(Server *server, string const &channel_name, string &user, int &fd);
		void							cleanup();

	private:
		// Attributes
		string							_name;
		string							_error_msg;
		list<string>					_channel_name;
		list<string>					_channel_key;
		vector<pair<string, string> >	_channel_vector;
};

#endif