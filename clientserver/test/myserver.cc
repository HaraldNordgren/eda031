/* myserver.cc: sample server program */
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <map>

using namespace std;

void listGroups(const shared_ptr<Connection>& conn);
void write_string_p(const shared_ptr<Connection>& conn, string s);
void write_num_p(const shared_ptr<Connection>& conn, unsigned char c);

/*
 * Read an integer from a client.
 */
int readNumber(const shared_ptr<Connection>& conn) {
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	
	/* 1094861636 means ABCD */

	/*cout << byte1 << "\\ ";
	cout << byte2 << "\\ ";
	cout << byte3 << "\\ ";
	cout << byte4 << endl;*/

	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

/*
 * Send a string to a client.
 */
void writeString(const shared_ptr<Connection>& conn, const string& s) {
	for (char c : s) {
		conn->write(c);
	}
	conn->write('$');
}

int main(int argc, char* argv[]){
	if (argc != 2) {
		cerr << "Usage: myserver port-number" << endl;
		exit(1);
	}
	
	int port = -1;
	try {
		port = stoi(argv[1]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}
	
	Server server(port);
	if (!server.isReady()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}
	

	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				unsigned char com = conn->read();
				switch (com) {
				case Protocol::COM_LIST_NG:
					conn->read();
					listGroups(conn);
					conn->write(Protocol::ANS_END);
					break;
				}
					



				/*int nbr = readNumber(conn);
				string result;
				if (nbr > 0) {
					result = "positive";
				} else if (nbr == 0) {
					result = "zero";
				} else {
					result = "negative";
				}
				writeString(conn, result);*/
			} catch (ConnectionClosedException&) {
				server.deregisterConnection(conn);
				cout << "Client closed connection" << endl;
			}
		} else {
			conn = make_shared<Connection>();
			server.registerConnection(conn);
			cout << "New client connects" << endl;
		}
	}
}

void listGroups(const shared_ptr<Connection>& conn) {
	conn->write(Protocol::ANS_LIST_NG);
	
	map<unsigned, string> groups;
	groups.insert({1,"football"});
	groups.insert({2,"hockey"});
	
	write_num_p(conn, groups.size());
	for (pair<unsigned,string> p : groups) {
		write_num_p(conn, p.first);
		write_string_p(conn, p.second);
	}
}

void write_string_p(const shared_ptr<Connection>& conn, string s) {
	conn->write(Protocol::PAR_STRING);
	conn->write(s.size());
	for (char c : s) {
		conn->write(c);
	}
}

void write_num_p(const shared_ptr<Connection>& conn, unsigned char c) {
	conn->write(Protocol::PAR_NUM);
	conn->write(c);
}
