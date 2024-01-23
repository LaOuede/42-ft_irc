#include "Server.hpp"
#include "CommandHandler.hpp"

#define WELCOME(hostname, nickname, username) ":" + hostname + " 001 " + nickname + " :Welcome, " + nickname + "!" + username + "@" + hostname + "\r\n"

extern bool g_running;

#define ERR_SERVERFULL "400 :No empty server slot\r\n"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Server::Server() {}

Server::Server(string port, string password) :
	_reuse(1), _socket_fd(0), _client_fd(0), _client_index(0), _nfds(0) {
	_port = atoi(port.c_str());
	_password = password;
	cout << "Server constructor call" << endl;
}

Server::~Server() {
	cout << "Server destructor call" << endl;
	cleanup();
	closeFds();
}

/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */
int &Server::getClientFd() {
	return this->_client_fd;
}

int &Server::getSocketFd() {
	return this->_socket_fd;
}

string const &Server::getCommandReceived() const {
	return this->_command_received;
}

map <int, clientInfo> &Server::getUserDB() {
	return this->_userDB;
}

uint32_t &Server::getClientIndex() {
	return this->_client_index;
}

CommandHandler &Server::getCommandHandler() {
	return this->_command_handler;
}

string &Server::getHostname() {
	return this->_hostname;
}

struct pollfd *Server::getFds() {
	return this->_fds;
}

string &Server::getPassword() {
	return this->_password;
}


map<string, Channel *> &Server::getChannelList() {
	return this->_channel_list;
}

Channel *Server::getChannel(string const &channel_name) {
	return this->_channel_list[channel_name];
}


/* ************************************************************************** */
/* Functions                                                                  */
/* ************************************************************************** */
void Server::createSocket() {
	this->_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->_socket_fd == -1)
		socketFailureException();
	cout << "Server created! socket_fd: " << this->_socket_fd << endl;
}

void Server::setSocket() {
	if ((setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR, &this->_reuse, sizeof(this->_reuse)) == -1)
		|| (fcntl(this->_socket_fd, F_SETFL, O_NONBLOCK) == -1))
		setsockoptFailureException();
	cout << "Server set! socket_fd: " << this->_socket_fd << endl;
}

void Server::bindSocket() {
	memset(&this->_sa, 0, sizeof this->_sa);
	this->_sa.sin_family = AF_INET;
	this->_sa.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	this->_sa.sin_port = htons(this->_port);
	if (bind(this->_socket_fd, (struct sockaddr *)&this->_sa, sizeof this->_sa) == -1)
		bindFailureException();
	cout << "Bound socket to localhost port: " << this->_port << endl;
	char hostname[1024];
	gethostname(hostname, 1024);
	_hostname = static_cast<string>(hostname);
}

void Server::socketListening() {
	if (listen(this->_socket_fd, BACKLOG) != 0) {
		listenFailureException();
	}
	cout << "Listening on port: " << this->_port << endl;
	cout << endl;
	cout << endl;
	cout << "To connect to the server, use the following commands:" << endl;
	cout << "/server add IRCserv host.docker.internal/6667 -notls" << endl;
	cout << "to set the password on your client, type this:" << endl;
	cout << "/set irc.server.IRCserv.password pass" << endl;
	cout << "/connect IRCserv" << endl;
}

void Server::serverRoutine(){
	initPollfd();
	while(g_running){
		if(poll(this->_fds, MAXFDS, 100) == 1){
			for(this->_client_index = 0; this->_client_index < MAXFDS; this->_client_index++){
				if(this->_client_index == 0 && this->_fds[this->_client_index].revents & POLLIN){
					acceptConnection();
				}else if(_fds[this->_client_index].revents & POLLIN)
					receiver();
			}
		}

		// only for visualition of the fd
		// for(int i = 0; i < MAXFDS; i++)
		// 	cout << "i : "<< i << " -> " <<_fds[i].fd << endl;
		// sleep(1);
	}
}

void Server::initPollfd() {
	this->_fds[0].fd = this->_socket_fd;
	this->_fds[0].events = POLLIN;
	this->_nfds++;
	for(int i = 1; i < MAXFDS; i++)
		this->_fds[i].fd = -1;
}

void Server::acceptConnection() {
	int status = accept(this->_socket_fd, 0, 0);
	if(status != -1){
		addNewClient(status);
	}else
		acceptFailureException(); // peut etre pas d'exception si on veux pas que le server ferme
}

void Server::addNewClient(int status) {
	for(uint32_t i = 0; i < MAXFDS; i++){
		if(_fds[i].fd == -1){
			_fds[i].fd = status;
			_fds[i].events = POLLIN;
			cout << "New connect #" << _fds[i].fd << endl;
			initBaseUser(status, i);
			return;
		}
	}
	send(status, ERR_SERVERFULL, strlen(ERR_SERVERFULL), 0);
	close(status);
}

void Server::initBaseUser(int status, int i)
{
	if(status == 4)
	{
		_userDB[_fds[i].fd]._nickname = "Emman";
		_userDB[_fds[i].fd]._realname = "Emmanuel Lamothe";
		_userDB[_fds[i].fd]._username = "Emman";
		_userDB[_fds[i].fd]._password_valid = true;
		_userDB[_fds[i].fd]._welcomed = true;
	}
	if(status == 5)
	{
		_userDB[_fds[i].fd]._nickname = "Prez";
		_userDB[_fds[i].fd]._realname = "Francis Bouchard";
		_userDB[_fds[i].fd]._username = "Prez";
		_userDB[_fds[i].fd]._password_valid = true;
		_userDB[_fds[i].fd]._welcomed = true;
	}
	if(status == 6)
	{
		_userDB[_fds[i].fd]._nickname = "Gwen";
		_userDB[_fds[i].fd]._realname = "Gwenola LeRoux";
		_userDB[_fds[i].fd]._username = "Gwen";
		_userDB[_fds[i].fd]._password_valid = true;
		_userDB[_fds[i].fd]._welcomed = true;
	}
}

void Server::receiver() {
	if(getBuffer() == -1)
		return;
	processRequests();
}

int Server::getBuffer() {
	int bytes = 0;
	while(1){
		bzero(_buf, BUFFERSIZE);
		bytes = recv(_fds[this->_client_index].fd, _buf, BUFFERSIZE, 0);
		if(bytes > 0)
			_buffer.append(_buf, BUFFERSIZE);
		else if(bytes == 0)
			return closeConnection();
		else
			return 0;
	}
}

int Server::closeConnection() {
	cout << "Closing connection #" << _fds[_client_index].fd << endl;
	close(_fds[_client_index].fd);
	if(_userDB.find(_fds[_client_index].fd) != _userDB.end())
		_userDB.erase(_userDB.find(_fds[_client_index].fd));
	_fds[_client_index].fd = -1;
	return -1;
}

void Server::processRequests() {
	// _buffer.assign("NICK salut\r\nNICK\r\nNICK\r\n");
	if(_buf[0] != 0)
		return;
	while(_buffer.empty() == false){
		splitBuffer();
		messageHandler();
		_command_received.clear();
	}
}

void Server::splitBuffer() {
	size_t pos = 0;
	pos = _buffer.find("\n");
	buildCommandReceived(pos);
	trimBuffer(pos);
}

void Server::buildCommandReceived(size_t pos) {
	if(pos != std::string::npos)
		_command_received.assign(_buffer.substr(0, pos));
	if(_command_received.find("\r") != string::npos)
		_command_received.pop_back();
}

void Server::trimBuffer(size_t pos) {
	if(_buffer.find("\n", _buffer.find("\n") + 1) != string::npos)
		_buffer.assign(_buffer.substr(pos + 1));
	else
		_buffer.clear();
}

void Server::messageHandler() {
	string response;
	int &fd = this->_fds[this->_client_index].fd;

	cout << "Message received from client socket " << fd << ": " << this->_command_received << endl;
	this->_command_handler.commandTokenizer( this );
	response = this->_command_handler.sendResponse( this );
	if (response.size() > 0) {
		this->_bytes_sent = send(fd, response.c_str(), response.size(), 0);
	}
	if (this->_bytes_sent == -1)
		sendFailureException();
	else if (this->_bytes_sent == (int)response.size()) {
		cout << "Message sent to client socket " << fd << " to confirm reception" << endl;
	} else {
		cout << "Message partially sent to client socket " << fd << ": " << this->_bytes_sent << endl;
	}
	welcomeMessage();
}

void Server::welcomeMessage() {
	string &hostname = this->_hostname;
	int &fd = this->_fds[this->_client_index].fd;
	string &nickname = this->_userDB[this->_fds[this->_client_index].fd]._nickname;
	string &username = this->_userDB[this->_fds[this->_client_index].fd]._username;
	bool &passworded = this->_userDB[this->_fds[this->_client_index].fd]._password_valid;
	bool &welcomed = this->_userDB[this->_fds[this->_client_index].fd]._welcomed;
	//manque le mot de passe

	if (username != "" && nickname != "" && welcomed == false && passworded == true) {
		welcomed = true;
		string response = WELCOME(hostname, nickname, username);
		this->_bytes_sent = send(fd, response.c_str(), response.size(), 0);
		if (this->_bytes_sent == -1)
			sendFailureException();
	}
}

// DEGUG - Print command name
void Server::parseCommand() {
	size_t pos = this->_command_received.find_first_of(" ");
	if (pos == string::npos) {
		cout << "Command received: " << this->_command_received << endl;
	} else {
		this->_command_received = this->_command_received.substr(0, pos);
		cout << "Command received: " << this->_command_received << endl;
	}
}

void Server::cleanup() {
	this->_userDB.clear();
	cleanChannelList();
}

void Server::cleanChannelList() {
	map<string, Channel *>::iterator it;

	it = this->_channel_list.begin();
	for (; it != this->_channel_list.end(); it++ ) {
		delete it->second;
	}
	this->_channel_list.clear();
}

void	Server::closeFds() {
	for(int i = 0; i < MAXFDS; i++)
		if(_fds[i].fd != -1)
			close(_fds[i].fd);
}

/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
std::exception Server::socketFailureException() {
	throw std::runtime_error("socket() error");
}

std::exception Server::bindFailureException() {
	throw std::runtime_error("bind() error");
}

std::exception Server::listenFailureException() {
	throw std::runtime_error("listen() error");
}

std::exception Server::acceptFailureException() {
	throw std::runtime_error("accept() error");
}

std::exception Server::recvFailureException() {
	throw std::runtime_error("recv() error");
}

std::exception Server::sendFailureException() {
	throw std::runtime_error("send() error");
}

std::exception Server::setsockoptFailureException() {
	throw std::runtime_error("setsockopt() error");
}
