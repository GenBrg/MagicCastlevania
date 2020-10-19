#pragma once

/* 
 * Connection is a simple wrapper around a TCP socket connection.
 * You don't create 'Connection' objects yourself, rather, you
 * create a Client or Server object which will manage connection(s)
 * for you.
 * For example:

//simple server
int main(int argc, char **argv) {
	Server server("1337"); //start a server on port 1337
	while (true) {
		server.poll([](Connection *connection, Connection::Event evt){
			if (evt == Connection::OnRecv) {
				//extract and erase data from the connection's recv_buffer:
				std::vector< uint8_t > data = connection->recv_buffer;
				connection->recv_buffer.clear();
				//send to other connections:

			}
		},
		1.0 //timeout (in seconds)
		);
	}
}

//simple client
int main(int argc, char **argv) {
	Client client("localhost", "1337"); //connect to a local server at port 1337
	while (true) {
		client.poll([](Connection *connection, Connection::Event evt){
			
		},
		0.0 //timeout (in seconds)
		);
	}
}

*/

//NOTE: much of the sockets code herein is based on http-tweak's single-header http server
// see: https://github.com/ixchow/http-tweak

//--------- 'SOCKET' is of different types on different OS's -------
#ifdef _WIN32
#ifdef _WIN64
typedef unsigned __int64 SOCKET;
#else
typedef unsigned int SOCKET;
#endif
constexpr const SOCKET INVALID_SOCKET = -1;
#else

typedef int SOCKET;
constexpr const SOCKET INVALID_SOCKET = -1;
#endif
//--------- ---------------------------------- ---------

#include <vector>
#include <list>
#include <string>
#include <functional>

//Thin wrapper around a (polling-based) TCP socket connection:
struct Connection {
	//Helper that will append any type to the send buffer:
	template< typename T >
	void send(T const &t) {
		send_raw(&t, sizeof(T));
	}
	//Helper that will append raw bytes to the send buffer:
	void send_raw(void const *data, size_t size) {
		send_buffer.insert(send_buffer.end(), reinterpret_cast< uint8_t const * >(data), reinterpret_cast< uint8_t const * >(data) + size);
	}

	//Call 'close' to mark a connection for discard:
	void close();

	//so you can if(connection) ... to check for validity:
	explicit operator bool() { return socket != INVALID_SOCKET; }

	//To send data over a connection, append it to send_buffer:
	std::vector< char > send_buffer;
	//When the connection receives data, it is appended to recv_buffer:
	std::vector< char > recv_buffer;

	//internals:
	SOCKET socket = INVALID_SOCKET;

	enum Event {
		OnOpen,
		OnRecv,
		OnClose
	};
};

struct Server {
	Server(std::string const &port); //pass the port number to listen on, as a string (servname, really)

	//poll() updates the list of active connections and provides information to your callbacks:
	void poll(
		std::function< void(Connection *, Connection::Event event) > const &connection_event = nullptr,
		double timeout = 0.0 //timeout (seconds)
	);

	std::list< Connection > connections;
	SOCKET listen_socket = INVALID_SOCKET;
};


struct Client {
	Client(std::string const &host, std::string const &port);

	//poll() checks the status of the active connection and provides information to your callbacks:
	void poll(
		std::function< void(Connection *, Connection::Event event) > const &connection_event = nullptr,
		double timeout = 0.0 //timeout (seconds)
	);

	std::list< Connection > connections; //will only ever contain exactly one connection
	Connection &connection; //reference to the only connection in the connections list
};
