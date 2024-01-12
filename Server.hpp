
#ifndef SERVER_HPP
#define SERVER_HPP

#pragma once

#include <exception>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
//poll test
#include <sys/poll.h>
#include <vector>

#define PORT 6667
#define BACKLOG 20
#define MAXCLIENT 2

#define WELCOME "001 user Welcome !\r\n"

using std::cout;
using std::endl;


class Server {
	public:
		// Construtors & Destructors
		Server();
		~Server();

		// Getters & Setters
		int &get_client_fd();
		int &get_socket_fd();

		// Methods
		void createSocket();
		void setSocket();
		void bindSocket();
		void socketListening();
		void serverRoutine();
		void initPollfd();
		void acceptConnection();
		void addNewClient(int status);

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
		int _reuse;
		int _socket_fd;
		int _client_fd;
		struct sockaddr_in _sa;
		struct sockadr_storage *_client_addr;
		socklen_t _addr_size;
		char _buf[BUFSIZ];
		int _bytes_read;
		int _bytes_sent;
		int _port;
		std::string _password;

		//TODO poll testing
		struct pollfd _fds[MAXCLIENT + 1]; // +1 for the socket_fd
		nfds_t _nfds;



};

#endif