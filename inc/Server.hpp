#ifndef SERVER_HPP
#define SERVER_HPP

#pragma once

#include "ACommand.hpp"
#include "Channel.hpp"
#include "CommandHandler.hpp"
#include <cctype>
#include <csignal>
#include <ctime>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <netdb.h>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#define C_BLU "\e[34m"
#define C_BOL "\e[1m"
#define C_ITA "\e[3m"
#define C_GRN "\e[32m"
#define C_RED "\e[31m"
#define C_WHT "\e[0m"

#define BUFFERSIZE 512
#define FLOODTIMELIMIT 1
#define FLOODCOUNTLIMIT 10
#define MAXCHANNEL 10
#define MAXCLIENT 40
#define MAXFDS (MAXCLIENT + 1) // +1 for the socket_fd
#define MAXMSGLEN 512
#define PORT 6667

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::isprint;
using std::map;
using std::runtime_error;
using std::string;

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
		void					clientInterfaceConnection();
		void					serverRoutine();
		void					initPollfd();
		void					acceptConnection();
		void					addNewClient(int status);
		void					initBaseUser(int status, int i);
		void 					receiver();
		int						buffering(string &buffer);
		int						closeConnection();
		int						inputTooLongError(string &buffer);
		void					floodProtection();
		bool					parseBuffer(string &buffer);
		void 					processRequests(string &buffer);
		void 					splitBuffer(string &buffer);
		void 					buildCommandReceived(size_t pos, string &buffer);
		void 					trimBuffer(size_t pos, string &buffer);
		void					messageHandler();
		void					welcomeMessage();
		void					cleanup();
		void					cleanChannelList();
		void					sendToClient(const string &response);
		void					closeFds();
		void					closeChannelFds();
		bool					isChannelEmpty(Channel *channel);
		void 					broadcastUserQuitMessage(Channel *channel, const string &user);
		bool					isNickInServer(string nickname);
		bool					isChannelInServer(string channelName);
		void					deleteChannel(Channel *channel);

		// Exceptions
		exception				socketFailureException();
		exception				bindFailureException();
		exception				listenFailureException();
		exception				newFailureException();
		exception				setsockoptFailureException();

	private:
		// Attributes
		int 					_reuse;
		int 					_socket_fd;
		uint32_t 				_client_index;
		struct sockaddr_in 		_sa;
		struct sockadr_storage 	*_client_addr;
		socklen_t 				_addr_size;
		char 					_buf[BUFFERSIZE];
		int 					_bytes_read;
		int 					_bytes_sent;
		int 					_port;
		string 					_password;
		CommandHandler			_command_handler;
		string					_command_received;
		string					_hostname;
		map<int, clientInfo>	_userDB;
		struct pollfd			_fds[MAXFDS];
		map<string, Channel *>	_channel_list;
};

#include "CommandHandler.hpp"

#endif
