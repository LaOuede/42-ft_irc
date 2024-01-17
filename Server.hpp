

#ifndef SERVER_HPP
#define SERVER_HPP

#pragma once

#include "ACommand.hpp"
#include "CommandHandler.hpp"
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <map>
#include <cctype>
//poll test
#include <sys/poll.h>
#include <vector>


#define PORT 6667
#define BACKLOG 20
#define MAXCLIENT 2
#define BUFFERSIZE 5


#define WELCOME "001 user Welcome !\r\n"

using std::cout;
using std::endl;
using std::string;
using std::map;

struct	clientInfo {
	string	_nickname;
	string	_username;
};

class CommandHandler;

class Server {
	public:
		// Construtors & Destructors
		Server();
		Server(string port, string password);
		~Server();

		// Getters & Setters
		int &get_client_fd();
		int &get_socket_fd();
		string const &get_command_received() const;
		map <int, clientInfo> &get_userDB();
		map <int, clientInfo> set_userDB(map <int, clientInfo> userDB);
		uint32_t &get_client_index();
		CommandHandler &get_command_handler();
		string &get_hostname();

		// Methods
		void createSocket();
		void setSocket();
		void bindSocket();
		void socketListening();
		void serverRoutine();
		void initPollfd();
		void acceptConnection();
		void addNewClient(int status);
		void messageHandler();
		void parseCommand();

		void receiver(int i);
		void buildCommandReceived();
		// Exceptions
		std::exception socketFailureException();
		std::exception bindFailureException();
		std::exception listenFailureException();
		std::exception acceptFailureException();
		std::exception recvFailureException();
		std::exception sendFailureException();
		std::exception setsockoptFailureException();

	private:
		// Attributes
		int 					_reuse;
		int 					_socket_fd;
		int 					_client_fd;
		uint32_t 				_client_index;
		struct sockaddr_in 		_sa;
		struct sockadr_storage 	*_client_addr;
		socklen_t 				_addr_size;
		char 					_buf[BUFFERSIZE];
		int 					_bytes_read;
		int 					_bytes_sent;
		int 					_port;
		string 					_password;
		CommandHandler 			_command_handler;
		string _buff;
		string 					_command_received;
		string					_hostname;

		map<int, clientInfo> 	_userDB;

		//TODO poll testing
		struct pollfd 			_fds[MAXCLIENT + 1]; // +1 for the socket_fd
		nfds_t 					_nfds;


};

#include "CommandHandler.hpp"

#endif