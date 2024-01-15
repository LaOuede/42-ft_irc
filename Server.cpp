#include "Server.hpp"
#include "CommandHandler.hpp"

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
					//need generic receving/parsing function here
				if(recv(_fds[i].fd, this->_buf, BUFSIZ, 0) != -1){
						//cout << "sent from connection #" << _fds[i].fd << " " << _buf;
						this->_command_received = this->_buf;
						messageHandler(i);
						bzero(_buf, BUFSIZ);
					}else
						recvFailureException();
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
	//send(_fds[_nfds].fd, WELCOME, 25, 0);
	_nfds++;
}

void Server::messageHandler(int i) {
	string response;

	cout << "\nMessage received from client socket " << this->_fds[i].fd << ": " << this->_command_received << endl;
	this->_command_handler.commandTokenizer( this );
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
