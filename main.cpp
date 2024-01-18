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

	cout << "---------- I'M THE SERVER ----------" << endl;

	Server server(argv[1], argv[2]);

	try
	{
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
	cout << endl << "Closing Server ..." << endl;
	return 0;
}