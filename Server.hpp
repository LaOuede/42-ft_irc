#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <unistd.h>

using std::cout;
using std::endl;

class Server
{
	public:
		Server();
		Server(Server const& copy);
		Server & operator=(Server const& copy);
		~Server();

		void start();

	private:
		uint32_t _port;
		uint32_t _ip;
};