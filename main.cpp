#include "Server.hpp"
#include <csignal>

bool g_running = true;

bool parsingPort(string port);
bool parsingPassword(string password);
void shutdown(int sig);

int main(int argc, char **argv) {
	signal(SIGINT, shutdown);
	signal(SIGQUIT, SIG_IGN);
	
	if (argc != 3) {
		cout << "Usage: ./ircserver <port> <password>" << endl;
		return 1;
	}

	if (!parsingPort(argv[1]) || !parsingPassword(argv[2])) {
		cout << "Error: Invalid arguments." << endl;
		return 1;
	}

	cout << C_BOL "\n--------------------- WELCOME ON IRCserv 🎙 ---------------------\n" C_WHT << endl;

	Server server(argv[1], argv[2]);

	try
	{
		cout << "Server initialization in progress..." << endl;
		server.createSocket();
		server.setSocket();
		server.bindSocket();
		server.socketListening();
		server.serverRoutine();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << endl;
	}
	cout << endl << "\nClosing Server ..." << endl;
	cout << C_BOL "------------- So long and thanks for all the fish 🐋 ------------\n" C_WHT << endl;
	return 0;
}