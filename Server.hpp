#include <iostream>

using std::cout;
using std::endl;

class Server
{
	public:
		Server();
		Server(Server const& copy);
		Server & operator=(Server const& copy);
		~Server();

	private:
		uint32_t _port;
		uint32_t _ip;
};