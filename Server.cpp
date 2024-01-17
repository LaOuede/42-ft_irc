#include "Server.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Server::Server() {}

Server::Server(string port, string password) :
	_reuse(1), _socket_fd(0), _client_fd(0), _nfds(0) {
	_port = atoi(port.c_str());
	_password = password;
	cout << "Server constructor call" << endl;
}

Server::~Server() {
	cout << "Server destructor call" << endl;
}


/* ************************************************************************** */
/* Getters & Setters                                                          */
/* ************************************************************************** */
int &Server::get_client_fd() {
	return this->_client_fd;
}

int &Server::get_socket_fd() {
	return this->_socket_fd;
}

string const &Server::get_command_received() const {
	return this->_command_received;
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
}

void Server::socketListening() {
	if (listen(this->_socket_fd, BACKLOG) != 0) {
		listenFailureException();
	}
	cout << "Listening on port: " << this->_port << endl;
}

void Server::serverRoutine(){
	initPollfd();
	while(1){
		if(poll(_fds, _nfds, 100) == 1){
			for(uint32_t i = 0; i < _nfds; i++){
				if(i == 0 && _fds[i].revents & POLLIN){
					acceptConnection();
				}else if(_fds[i].revents & POLLIN){
						// if(_buffer.empty() == true)
					receiver(i);
					_buffer.assign("NICK\r\nNICK\r\n");
					while(_buffer.empty() == false){
						buildCommandReceived();
						cout << "sent from connection #" << _fds[i].fd << ": " << _command_received;
						messageHandler(i);
						_command_received.clear();
						cout << "2" << endl;
						sleep(1);
					}
					_command_received.clear();
					cout << "1" << endl;
					sleep(1);
				}
			}
		}
		// only for visualition of the fd
/* 		for(int i = 0; i < MAXCLIENT + 1; i++)
			cout << "i : "<< i << " -> " <<_fds[i].fd << endl;
		sleep(1); */
	}
}

void Server::initPollfd(){
	_fds[0].fd = _socket_fd;
	_fds[0].events = POLLIN;
	_nfds++;
	for(int i = 1; i < MAXCLIENT + 1; i++)
		_fds[i].fd = -1;
}

void Server::acceptConnection() {
	int status = accept(this->_socket_fd, 0, 0);
	if(status != -1){
		// besoin d'un recv pour save les info NICK/USER
		if(_nfds < MAXCLIENT + 1){
			addNewClient(status);
		}else{
			//marche pas full bien
			send(status, "Server is full comeback later \r\n", 50, 0);
			close(status);
		}
	}else
		acceptFailureException(); // peut etre pas d'exception si on veux pas que le server ferme
}

void Server::addNewClient(int status){
	_fds[_nfds].fd = status;
	_fds[_nfds].events = POLLIN;
	cout << "New connect #" << _fds[_nfds].fd << endl;
	// send(_fds[_nfds].fd, WELCOME, 25, 0);
	_nfds++;
}

void Server::receiver(int i)
{
	int bytes = 0;
	// char test[] = "allo\nbonjour\n";

	while(1){
		bzero(_buf, BUFFERSIZE);
		bytes = recv(_fds[i].fd, _buf, BUFFERSIZE, 0);
		cout << "bytes " << bytes << endl;
		if(bytes != -1)
			_buffer.append(_buf, BUFFERSIZE);
		else
			break;
		sleep(1);
	}
}

void Server::buildCommandReceived(){
	size_t pos = 0;
	pos = _buffer.find("\n");
	if(pos != std::string::npos)
		_command_received.assign(_buffer.substr(0, pos));
	if(_command_received.find("\r") != string::npos)
		_command_received.pop_back();
	if(_buffer.find("\n", _buffer.find("\n") + 1) != string::npos)
		_buffer.assign(_buffer.substr(pos + 1));
	else{
		cout << "clear" << endl;
		_buffer.clear();
	}
	cout << "_command_received " << _command_received << endl;
	cout << "buff "<< _buffer << endl;
}

void Server::messageHandler(int i) {
	string response;

	cout << "Message received from client socket " << this->_fds[i].fd << ": " << this->_command_received << endl;
	parseCommand();
	response = this->_command_handler.sendResponse( this );
	if (response.size() > 0) {
		this->_bytes_sent = send(this->_fds[i].fd, response.c_str(), response.size(), 0);
	}
	if (this->_bytes_sent == -1)
		sendFailureException();
	else if (this->_bytes_sent == (int)response.size()) {
		cout << "Message sent to client socket " << this->_fds[i].fd << " to confirm reception" << endl;
	} else {
		cout << "Message partially sent to client socket " << this->_fds[i].fd << ": " << this->_bytes_sent << endl;
	}
}

void Server::parseCommand() {
	size_t pos = this->_command_received.find_first_of(" ");
	if (pos == string::npos) {
		cout << "Command received: " << this->_command_received << endl;
	} else {
		this->_command_received = this->_command_received.substr(0, pos);
		cout << "Command received: " << this->_command_received << endl;
	}
}

/* ************************************************************************** */
/* Exceptions                                                                 */
/* ************************************************************************** */
std::exception Server::socketFailureException(){
	throw std::runtime_error("socket() error");
}

std::exception Server::bindFailureException(){
	throw std::runtime_error("bind() error");
}

std::exception Server::listenFailureException(){
	throw std::runtime_error("listen() error");
}

std::exception Server::acceptFailureException(){
	throw std::runtime_error("accept() error");
}

std::exception Server::recvFailureException(){
	throw std::runtime_error("recv() error");
}

std::exception Server::sendFailureException(){
	throw std::runtime_error("send() error");
}

std::exception Server::setsockoptFailureException(){
	throw std::runtime_error("setsockopt() error");
}
