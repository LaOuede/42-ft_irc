#include "Server.hpp"

/* ************************************************************************** */
/* Constructors and Destructors                                               */
/* ************************************************************************** */
Server::Server() {}

Server::Server(string port, string password) :
	_reuse(1), _socket_fd(0), _client_fd(0) {
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
		/*|| (fcntl(this->_socket_fd, F_SETFL, O_NONBLOCK) == -1)*/)
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

void Server::acceptConnection() {
	cout << "socket_fd "<< this->_socket_fd << endl;
	this->_client_fd = accept(this->_socket_fd, 0, 0);
	if (this->_client_fd == -1)
		acceptFailureException();
	cout << "New connection accepted on client socket fd: " << this->_client_fd << endl;
}

void Server::serverRoutine() {
	
	this->_bytes_read = 1;
	while (this->_bytes_read >= 0) {
		cout << "Reading client socket: "<< this->_client_fd << endl;
		this->_bytes_read = recv(this->_client_fd, this->_buf, BUFSIZ, 0);
		if (this->_bytes_read == 0) {
			cout << "Client socket " << this->_client_fd << ": connection closed" << endl;
			break;
		} else if (this->_bytes_read == -1)
			recvFailureException();
		else {			
			messageHandler();
		}
	}
}

void Server::messageHandler() {
	// récupérer le message du client
	// split (attention, plusieurs commandes peuvent se suivre séparées par "/r/n")
	// répondre en fonction de ce qui a été reçu.

	// Récupérer les infos de NICK et USER quand reçu
	// Renvoyer "001 <user> Welcome"

	string response;
	this->_buf[this->_bytes_read] = '\0';
	cout << "Message received from client socket " << this->_client_fd << ": " << this->_buf << endl;
	parseCommand();
	response = this->_command_handler.sendResponse( this );
	if (response.size() > 0) {
		this->_bytes_sent = send(this->_client_fd, response.c_str(), response.size(), 0);
	}
	if (this->_bytes_sent == -1)
		sendFailureException();
	else if (this->_bytes_sent == (int)response.size()) {
		cout << "Message sent to client socket " << this->_client_fd << " to confirm reception" << endl;
	} else {
		cout << "Message partially sent to client socket " << this->_client_fd << ": " << this->_bytes_sent << endl;
	}
}

void Server::parseCommand() {
	size_t pos = static_cast<string>(this->_buf).find_first_of(" ");
	if (pos == string::npos) {
		this->_command_received = this->_buf;
		cout << "Command received: " << this->_command_received << endl;
	} else {
		this->_command_received = static_cast<string>(this->_buf).substr(0, pos);
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
