#ifndef SERVER_HPP
#define SERVER_HPP

#pragma once

#include "ACommand.hpp"
#include "CommandHandler.hpp"
#include "Channel.hpp"
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
#include <sys/poll.h>
#include <vector>
#include <csignal>
#include <ctime>

#define PORT 6667
#define BACKLOG 20
#define MAXCLIENT 10
#define MAXFDS (MAXCLIENT + 1) // +1 for the socket_fd
#define BUFFERSIZE 512
#define MAXMSGLEN 512
#define MAXCHANNEL 10
#define FLOODCOUNTLIMIT 10
#define FLOODTIMELIMIT 1

using std::cout;
using std::endl;
using std::string;
using std::map;

struct	clientInfo {
	string	_nickname;
	string	_username;
	string	_realname;
	bool	_password_valid;
	bool	_welcomed;
	int		_nb_channel;
	string	_buffer;
	int		_floodCount;
	time_t	_lastTime;
};

class CommandHandler;

class Server {
	public:
		// Construtors & Destructors
		Server();
		Server(string port, string password);
		~Server();

		// Getters & Setters
		int						&getClientFd();
		int						&getSocketFd();
		string const 			&getCommandReceived() const;
		map <int, clientInfo>	&getUserDB();
		uint32_t				&getClientIndex();
		CommandHandler			&getCommandHandler();
		string					&getHostname();
		struct pollfd			*getFds();
		string					&getPassword();
		map<string, Channel *>	&getChannelList();
		Channel 				*getChannel(string const &channel_name);

		// Methods
		void					createSocket();
		void					setSocket();
		void					bindSocket();
		void					socketListening();
		void					serverRoutine();
		void					initPollfd();
		void					acceptConnection();
		void					addNewClient(int status);
		// void					initBaseUser(int status, int i);
		void 					receiver();
		int						getBuffer(string &buffer);
		int						closeConnection();
		int						inputTooLongError(string &buffer);
		void					floodProtection();
		bool					parseBuffer(string &buffer);
		void 					processRequests(string &buffer);
		void 					splitBuffer(string &buffer);
		void 					buildCommandReceived(size_t pos, string &buffer);
		void 					trimBuffer(size_t pos, string &buffer);
		void					messageHandler();
		void					parseCommand();
		void					welcomeMessage();
		void					cleanup();
		void					cleanChannelList();
		void					sendToClient(string *response);
		void					closeFds();
		void					closeChannelFds();
		bool					isChannelEmpty(Channel *channel);

		// Exceptions
		std::exception			socketFailureException();
		std::exception			bindFailureException();
		std::exception			listenFailureException();
		std::exception			acceptFailureException();
		std::exception			recvFailureException();
		std::exception			sendFailureException();
		std::exception			setsockoptFailureException();

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

		string					_command_received;
		string					_hostname;
		map<int, clientInfo>	_userDB;
		struct pollfd			_fds[MAXFDS];
		nfds_t					_nfds;
		map<string, Channel *>	_channel_list;
};

#include "CommandHandler.hpp"

#endif

