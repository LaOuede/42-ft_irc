
#include "Server.hpp"

int main(int argc, char **argv) {
	(void)argv;
	if (argc != 3) {
		cout << "Usage: ./ircserver <port> <password>" << endl;
		return 1;
	}
	cout << "---------- I'M THE SERVER ----------" << endl;

	Server server;

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

	cout << "Closing client socket..." << endl;
	close(server.get_client_fd());
	cout << "Closing server socket..." << endl;
	close(server.get_socket_fd());
	return 0;
}