#ifndef PART_HPP
#define PART_HPP

#pragma once

# include "ACommand.hpp"
# include <iostream>
# include <list>
# include <string>

using std::cout;
using std::endl;
using std::list;
using std::string;

class Server;

class Part : public ACommand {
	public:
		// Construtors & Destructors
		Part();
		virtual ~Part();

		// Methods
		string executeCommand(Server *server);
		string parseCommand(Server *server);
		string parseParameters(const list<string> &command);
		string parseAttributes(const list<string> &command);
		void splitParameters(string to_split, list<string> &to_fill);
		string processChannelDeconnections(Server *server);
		void cleanup();

	private:
		// Attributes
		string				_name;
		string				_error_msg;
		list<string>		_channel_name;

};

#endif