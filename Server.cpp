#include "Server.hpp"

Server::Server()
{
	cout << "Server Constructor call" << endl;
}

Server::Server(Server const& copy)
{
	cout << "Server copy Constructor call" << endl;
}

Server & Server::operator=(Server const& copy)
{
	cout << "Server assignement call" << endl;
	return *this;
}

Server::~Server()
{

}

void Server::start()
{
	struct sockaddr_in serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = 6667;
	
	int serverSocket;
	int acceptedSocket;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serverSocket == -1)
		cout << "error " << endl;
	else
		cout << "socket ok" <<  endl;
	if(bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		cout << "erreur de bind" <<  endl;
		close(serverSocket);
	}
	else
		cout << "bind ok" <<  endl;
	if(listen(serverSocket, 1) == -1)
	{
		cout << "listen error" <<  endl;
		close(serverSocket);
	}
	else
		cout << "listening ok" <<  endl;
	if(accept(serverSocket, NULL, NULL) == -1)
		cout << "accept error" <<  endl;
	else
		cout << "accept ok" <<  endl;
}