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
}

Server::~Server()
{

}