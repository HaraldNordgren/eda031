/* myclient.cc: sample client program */
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>

using namespace std;

unsigned char read_num_p(const Connection& conn);
string read_string_p(const Connection& conn);

/*
 * Send an integer to the server as four bytes.
 */
void writeNumber(const Connection& conn, int value) {
	conn.write((value >> 24) & 0xFF);
	conn.write((value >> 16) & 0xFF);
	conn.write((value >> 8)	 & 0xFF);
	conn.write(value & 0xFF);
}

int readNumber(const Connection& conn) {
	unsigned char byte1 = conn.read();
	unsigned char byte2 = conn.read();
	unsigned char byte3 = conn.read();
	unsigned char byte4 = conn.read();
	
	return (byte1 << 24) | (byte2 << 16) | (byte3 << 8) | byte4;
}

/*
 * Read a string from the server.
 */
string readString(const Connection& conn) {
	string s;
	char ch;
	while ((ch = conn.read()) != '$') {
		s += ch;
	}
	return s;
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cerr << "Usage: myclient host-name port-number" << endl;
		exit(1);
	}
	
	int port = -1;
	try {
		port = stoi(argv[2]);
	} catch (exception& e) {
		cerr << "Wrong port number. " << e.what() << endl;
		exit(1);
	}
	
	Connection conn(argv[1], port);
	if (!conn.isConnected()) {
		cerr << "Connection attempt failed" << endl;
		exit(1);
	}
	
	cout << "news> ";
	string command;
	while (cin >> command) {
		try {
			if (command == "list") {
				conn.write(Protocol::COM_LIST_NG);
				conn.write(Protocol::COM_END);
				
				conn.read();
				unsigned length = read_num_p(conn);
				for (unsigned i = 0; i < length; ++i) {
					unsigned num_p = read_num_p(conn);
					string string_p = read_string_p(conn);
					cout << num_p << ". " << string_p << endl;
				}
				conn.read();
			} else {
				cout << endl;
			}


			cout << "news> ";


			/*cout << nbr << " is ...";
			writeNumber(conn, nbr);
			string reply = readString(conn);
			cout << " " << reply << endl;
			cout << "Type another number: ";*/
		} catch (ConnectionClosedException&) {
			cout << " no reply from server. Exiting." << endl;
			exit(1);
		}
	}
}


unsigned char read_num_p(const Connection& conn) {
	conn.read();
	return readNumber(conn);
}

string read_string_p(const Connection& conn) {
	conn.read();
	unsigned N = readNumber(conn);
	
	string s;
	for (unsigned i = 0; i < N; ++i) {
		s += conn.read();
	}
	return s;
}
