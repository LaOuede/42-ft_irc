#include "Server.hpp"

extern bool g_running;

bool parsingPort(string port) {
	if (port.empty() || port.length() > 4
		|| port.find_first_not_of("0123456789") != string::npos
		|| atoi(port.c_str()) < 1024) {
		return false;
	}
	return true;
}

bool parsingPassword(string password) {
	string whitespaces = "\t\n\v\f\r ";

	if (password.empty() || password.length() > 10
		|| password.find_first_of(whitespaces) != string::npos) {
		return false;
	}
	return true;
}

void Server::sendToClient(const string &response) {
	if (send(_fds[_client_index].fd, response.c_str(), response.size(), 0) == -1)
		std::cerr << "Error : SEND return -1" << endl;
}

void shutdown(int sig){
	(void)sig;
	g_running = false;
}
