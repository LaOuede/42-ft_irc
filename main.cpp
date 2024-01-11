#include "Server.hpp"

bool parsingPort(string port);
bool parsingPassword(string password);

int main(int argc, char **argv) {
	(void)argv;
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
		server.newPollRoutine();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << endl;
	}

	//closing of the server
	cout << "Closing client socket..." << endl;
	close(server.get_client_fd());
	cout << "Closing server socket..." << endl;
	close(server.get_socket_fd());
	return 0;
}