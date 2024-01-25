#include "Server.hpp"
#include "CommandHandler.hpp"

extern bool g_running;
/* ************************************************************************** */
/* Defines                                                                    */
/* ************************************************************************** */
#define WELCOME(hostname, nickname, username) ":" + hostname + " 001 " + nickname + " :Welcome, " + nickname + "!" + username + "@" + hostname + "\r\n"
#define ERR_SERVERFULL "400 :No empty server slot\r\n"
# define ERR_INPUTTOOLONG "417 <client> :Input line was too long\r\n"
# define ERR_FLOOD "400 Disconnected : Flood protection, niaise pas avec moé !\r\n"
//# define RPL_QUITCHANNEL(function, user, channel) ": 400 " + function + " :" + user + " is leaving the channel '" + channel + "'\r\n"
# define RPL_QUITCHANNEL(user, channel) ":" + user + " PART " + channel + "\r\n"


/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Server::Server() {}

Server::Server(string port, string password) :
	_reuse(1), _socket_fd(0), _client_index(0), _nfds(0) {
	_port = atoi(port.c_str());
	_password = password;
}

Server::~Server() {
	cleanup();
	closeFds();
}


/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */
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
	cout << "	...Server has been created! " << endl;
}

void Server::setSocket() {
	if ((setsockopt(this->_socket_fd, SOL_SOCKET, SO_REUSEADDR, &this->_reuse, sizeof(this->_reuse)) == -1)
		|| (fcntl(this->_socket_fd, F_SETFL, O_NONBLOCK) == -1))
		setsockoptFailureException();
	cout << "	...Server has been set!" << endl;
}

void Server::bindSocket() {
	memset(&this->_sa, 0, sizeof this->_sa);
	this->_sa.sin_family = AF_INET;
	this->_sa.sin_addr.s_addr = htonl(INADDR_ANY);
	this->_sa.sin_port = htons(this->_port);
	if (bind(this->_socket_fd, (struct sockaddr *)&this->_sa, sizeof this->_sa) == -1)
		bindFailureException();
	cout << "	...Socket has been bound to localhost port: " << C_BOL C_BLU << this->_port << C_WHT << endl;
	char hostname[1024];
	gethostname(hostname, 1024);
	_hostname = static_cast<string>(hostname);
}

void Server::socketListening() {
	if (listen(this->_socket_fd, BACKLOG) != 0) {
		listenFailureException();
	}
	cout << "	...Server listening on port: " << C_BOL C_BLU << this->_port << C_WHT << endl;
	clientInterfaceConnection();
}

void Server::clientInterfaceConnection() {
	cout << "\n\n\n********************* " << C_BLU << "Connection process: " << C_WHT << "**********************" << endl;
	cout << "To connect to the server, use the following commands:\n" << endl;
	cout << "- On " << C_GRN "WeeChat:" C_WHT << endl;
	cout << C_ITA"    /server add IRCserv host.docker.internal/6667 -notls" C_WHT << endl;
	cout << C_BOL "  to set the password on your client, type this:" C_WHT << endl;
	cout << C_ITA "    /set irc.server.IRCserv.password pass" C_WHT << endl;
	cout << C_BOL "  to set your username, type this:" C_WHT << endl;
	cout << C_ITA "    /set irc.server.IRCserv.username User2" C_WHT << endl;
	cout << C_BOL "  to set your nickname, type this:" C_WHT << endl;
	cout << C_ITA "    /set irc.server.IRCserv.nicks allo1" C_WHT << endl;
	cout << C_ITA "  /connect IRCserv" << endl;
	cout << "\n- On " << C_GRN "NetCat:" C_WHT << endl;
	cout << C_ITA "    nc 127.0.0.1 6667" C_WHT << endl;
	cout << C_BOL "  to set the password on your client, type this:" C_WHT << endl;
	cout << C_ITA "    PASS pass" C_WHT << endl;
	cout << C_BOL "  to set your username, type this:" C_WHT << endl;
	cout << C_ITA "    USER user2 0 * :user2" C_WHT << endl;
	cout << C_BOL "  to set your nickname, type this:" C_WHT << endl;
	cout << C_ITA "    NICK allo1" C_WHT << endl;
	cout << "*****************************************************************\n" << endl;
	cout << C_RED C_BOL "Log: " << C_WHT << endl;
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
				else if(_fds[this->_client_index].revents & (POLLNVAL | POLLERR | POLLHUP)){
					cout << "(POLLNVAL | POLLERR | POLLHUP)" << endl;
					closeConnection();
				}
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
	for(int i = 1; i < MAXFDS; i++){
		this->_fds[i].fd = -1;
		_userDB[_fds[i].fd]._floodCount = 0;
	}
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
			cout << "New connect on socket #" << _fds[i].fd << endl;
			//initBaseUser(status, i);
			return;
		}
	}
	send(status, ERR_SERVERFULL, strlen(ERR_SERVERFULL), 0);
	close(status);
}

/* void Server::initBaseUser(int status, int i)
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
} */

void Server::receiver() {
	string &buffer = _userDB[_fds[_client_index].fd]._buffer;
	if(getBuffer(buffer) == -1){
		cout << "buffer : " << buffer << " / Client index : " << _client_index << endl;
		return;
	}
	if(buffer != "\n" && parseBuffer(buffer))
		processRequests(buffer);
	else
		buffer.clear();
}

int Server::getBuffer(string &buffer) {
	int bytes = 0;
	
	while(1){
		bzero(_buf, BUFFERSIZE);
		bytes = recv(_fds[this->_client_index].fd, _buf, BUFFERSIZE, 0);
		if(bytes == 0 || _userDB[_fds[this->_client_index].fd]._floodCount > FLOODCOUNTLIMIT){
			if(_userDB[_fds[this->_client_index].fd]._floodCount > 5)
				send(_fds[_client_index].fd, ERR_FLOOD, strlen(ERR_FLOOD), 0);
			return closeConnection();
		}
		else if(buffer.length() > MAXMSGLEN)
			return inputTooLongError(buffer);
		else if(bytes > 0)
			buffer.append(_buf, strlen(_buf));
		else if(buffer.find("\n") != string::npos){
			floodProtection();
			return 0;
		}
		else
			return -1;
	}
}

int Server::closeConnection() {
	cout << "Closing connection #" << _fds[_client_index].fd << endl;
	closeChannelFds();
	close(_fds[_client_index].fd);
	if(_userDB.find(_fds[_client_index].fd) != _userDB.end())
		_userDB.erase(_userDB.find(_fds[_client_index].fd));
	_fds[_client_index].fd = -1;
	return -1;
}

int	Server::inputTooLongError(string &buffer){
	send(this->_fds[this->_client_index].fd, ERR_INPUTTOOLONG, strlen(ERR_INPUTTOOLONG), 0);
	buffer.clear();
	_userDB[_fds[_client_index].fd]._floodCount++;
	return -1;
}

void Server::floodProtection(){
	time_t currentTime = time(nullptr);
		// cout << "current time :" << currentTime << endl;
		// cout << "last time :" << _userDB[_fds[_client_index].fd]._lastTime << endl;
	if(currentTime - _userDB[_fds[_client_index].fd]._lastTime > FLOODTIMELIMIT){
		_userDB[_fds[this->_client_index].fd]._floodCount = 0;
		_userDB[_fds[_client_index].fd]._lastTime = currentTime;
	}
	else
		_userDB[_fds[this->_client_index].fd]._floodCount++;
}

bool Server::parseBuffer(string &buffer) {
	for (string::iterator it = buffer.begin(); it != buffer.end(); it++)
		if (!std::isprint(static_cast<unsigned char>(*it)) && (*it != '\r' && *it != '\n'))
			return false;
	return true;
}

void Server::processRequests(string &buffer) {
	// _buffer.assign("NICK salut\r\nNICK\r\nNICK\r\n");
	if(_buf[0] != 0)
		return;
	while(buffer.empty() == false){
		splitBuffer(buffer);
		messageHandler();
		_command_received.clear();
	}
}

void Server::splitBuffer(string &buffer) {
	size_t pos = 0;
	pos = buffer.find("\n");
	buildCommandReceived(pos, buffer);
	trimBuffer(pos, buffer);
}

void Server::buildCommandReceived(size_t pos, string &buffer) {
	if(pos != std::string::npos)
		_command_received.assign(buffer.substr(0, pos));
	while(_command_received.find("\r") != string::npos || _command_received.find("\n") != string::npos)
		_command_received.pop_back();
}

void Server::trimBuffer(size_t pos, string &buffer) {
	if(buffer.find("\n", buffer.find("\n") + 1) != string::npos)
		buffer.assign(buffer.substr(pos + 1));
	else
		buffer.clear();
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

void Server::closeChannelFds() {
	list<string>::iterator delIt;
	map<string, Channel *>::iterator it;
	list<string> channelsToDelete;

	it = this->_channel_list.begin();
	for (; it != this->_channel_list.end(); it++) {
		if (it->second->isUserInChannel(this->_fds[this->_client_index].fd)) {
			broadcastUserQuitMessage(it->second, this->_userDB[_fds[_client_index].fd]._nickname);
			it->second->removeUserFromChannel(this, _fds[_client_index].fd);
		}
		if (isChannelEmpty(it->second)) {
			channelsToDelete.push_back(it->first);
		}
	}
	delIt = channelsToDelete.begin();
	for (; delIt != channelsToDelete.end(); ++delIt) {
		delete this->_channel_list[*delIt];
		this->_channel_list.erase(*delIt);
	}
}

void Server::broadcastUserQuitMessage(Channel *channel, const string &user) {
	const string &channel_name = channel->getChannelName();
	string msg = RPL_QUITCHANNEL(user, channel_name);
	channel->broadcastToAll(msg);
}

bool Server::isChannelEmpty(Channel *channel) {
	if (channel->getUsersNb() == 0 && channel->getOperatorsNb() == 0) {
		return true;
	}
	return false;
}

bool Server::isNickInServer(string nickname){
	map<int, clientInfo>::iterator it = _userDB.begin();
	for(; it != _userDB.end(); it++)
		if(it->second._nickname == nickname)
			return true;
	return false;
}

bool	Server::isChannelInServer(string channelName){
	if(_channel_list.find(channelName) != _channel_list.end()){
		cout << "channel trouver" << endl;
		return true;
	}
	cout << "channel NON trouver" << endl;
	return false;
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
