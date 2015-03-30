/* myserver.cc: sample server program */
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "disk_database.h"
#include "inmemory_database.h"
#include "database.h"

#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <map>

using namespace std;

const string DEFAULT_PATH = "./database/";

void listGroups(const shared_ptr<Connection>& conn, database& db);
void write_string_p(const shared_ptr<Connection>& conn, string s);
void write_num_p(const shared_ptr<Connection>& conn, unsigned char c);
void writeNumber(const shared_ptr<Connection>& conn, int value);

void enter_testdata(database& db);
void test1(database& db);

/*
 * Read an integer from a client.
 */
int readNumber(const shared_ptr<Connection>& conn) {
	unsigned char byte1 = conn->read();
	unsigned char byte2 = conn->read();
	unsigned char byte3 = conn->read();
	unsigned char byte4 = conn->read();
	
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

int main(int argc, char* argv[]) {
	if (argc != 2 && argc!= 3) {
		string long_name = argv[0];
		auto pos = long_name.rfind("/");

		cerr << "Usage: " << long_name.substr(pos+1) << " port-number [database-path]" << endl;
		exit(1);
	}
	
	int port = -1;
	try {
		port = stoi(argv[1]);
	} catch (exception& e) {
		cerr << "Wrong port number format " << e.what() << endl;
		exit(1);
	}
	
	Server server(port);
	if (!server.isReady()) {
		cerr << "Server initialization error." << endl;
		exit(1);
	}

	string db_path;	
	if (argc == 3) {
		db_path = argv[2];
		if (db_path[db_path.length()-1] != '/') {
			db_path += "/";
		}

	} else {
		db_path = DEFAULT_PATH;
	}

	disk_database db(db_path);
	//inmemory_database db;
	
	enter_testdata(db);
	//test1(db);

	
	cout << "Server running on port " << port << endl << endl;

	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				unsigned char com = conn->read();
				switch (com) {
				case Protocol::COM_LIST_NG:
					conn->read();
					listGroups(conn,db);
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

void listGroups(const shared_ptr<Connection>& conn, database& db) {
	conn->write(Protocol::ANS_LIST_NG);
	
	auto groups = db.list_newsgroups();
	write_num_p(conn, groups.size());
	for (pair<unsigned,string> p : groups) {
		write_num_p(conn, p.first);
		write_string_p(conn, p.second);
	}
}

void write_string_p(const shared_ptr<Connection>& conn, string s) {
	conn->write(Protocol::PAR_STRING);
	writeNumber(conn, s.size());
	for (char c : s) {
		conn->write(c);
	}
}

void write_num_p(const shared_ptr<Connection>& conn, unsigned char c) {
	conn->write(Protocol::PAR_NUM);
	writeNumber(conn, c);
}

void writeNumber(const shared_ptr<Connection>& conn, int value) {
	conn->write((value >> 24) & 0xFF);
	conn->write((value >> 16) & 0xFF);
	conn->write((value >> 8)  & 0xFF);
	conn->write(value & 0xFF);
}

void enter_testdata(database& db) {
	db.create_newsgroup("football");
	db.create_newsgroup("hockey");
	db.create_newsgroup("baseball");
	
	article art1 =  {"Why sports are fun", "harald", "Sports are fun because\n....."};
	article art2 =  {"Why sports are not fun", "johnny", "They are not\nfun because"};
	article art3 =  {"Why sports might be fun", "jack", "It is unknown,\nwhether or not sport are fun"};

	db.create_article(2,art1);
	db.create_article(2,art2);
	db.create_article(2,art3);

	db.delete_article(2,2);
}

void test1(database& db) {
	auto newsgroup = db.list_newsgroups();
	for (auto p : newsgroup) {
		cout << p.first << " " << p.second << endl;
	}

	cout << endl;

	auto articles = db.list_articles(2).second;
	for (pair<unsigned,string> p : articles) {
		cout << p.first << " " << p.second << endl;
	}

	cout << endl;

	article art = db.get_article(2,3).second;
	cout << "\"" << art.text << "\"" << endl << endl;
}
