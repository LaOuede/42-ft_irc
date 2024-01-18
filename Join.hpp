#ifndef JOIN_HPP
#define JOIN_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <list>
# include <map>
# include <string>

using std::cout;
using std::endl;
using std::list;
using std::map;
using std::string;

class Server;

class Join : public ACommand {
	public:
		// Construtors & Destructors
		Join();
		virtual ~Join();

		// Methods
		string executeCommand(Server *server);
		string parseCommand(Server *server);
		string parseParameters(const list<string> &command);
		void splitParameters(string to_split, list<string> &to_fill);
		string parseAttributes(const list<string> &command);
		void createChannelsMap();
		string processChannelsConnections(Server *Server);
		string parseChannelNameAndKey(string name, string key);
		void cleanup();

	private:
		// Attributes
		string				_name;
		string				_error_msg;
		list<string>		_channels_names;
		list<string>		_channels_keys;
		map<string, string>	_channels_map;

};

#endif