/* myserver.cc: sample server program */
#include "server.h"
#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "disk_database.h"
#include "inmemory_database.h"
#include "database.h"
#include "messagehandler.h"

#include <memory>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <map>

using namespace std;



const string DEFAULT_PATH = "./database/";

const bool debug_messages = true;	// Fult jag vet, ta gärna bort om det stör.
void debug_msg(const string msg);	// -||-

void list_groups(MessageHandler& mh, database& db);
void create_newsgroup(MessageHandler& mh, database& db);
void delete_newsgroup(MessageHandler& mh, database& db);
void list_articles(MessageHandler& mh, database& db);
void create_article(MessageHandler& mh, database& db);
void delete_article(MessageHandler& mh, database& db);
void get_article(MessageHandler& mh, database& db);

void enter_testdata(database& db);
void test1(database& db);


int main(int argc, char* argv[]) {
	if (argc != 2 && argc!= 3) {
		string long_name = argv[0];
		auto pos = long_name.rfind("/");

		cerr << "Usage: " << long_name.substr(pos+1) <<
			" port-number [database-path]" << endl;
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

	//disk_database db(db_path);
	inmemory_database db;
	
	//enter_testdata(db);
	//test1(db);

	
	cout << "Server running on port " << port << endl << endl;

	while (true) {
		auto conn = server.waitForActivity();
		if (conn != nullptr) {
			try {
				unsigned char com = conn->read();
				MessageHandler mh(conn);
				
				switch (com) {
					case Protocol::COM_LIST_NG:					
						debug_msg("COM_LIST_NG");		
						list_groups(mh, db);
						//debug_msg("-> exit case");
						break;
				
					case Protocol::COM_CREATE_NG:
						debug_msg("COM_CREATE_NG");
						create_newsgroup(mh, db);
						//debug_msg("-> exit case");
						break;
					
					case Protocol::COM_DELETE_NG:	
						debug_msg("COM_DELETE_NG; ");			
						delete_newsgroup(mh, db);
						//debug_msg("-> exit case");
						break;
						
					case Protocol::COM_LIST_ART:
						debug_msg("COM_LIST_ART");
						list_articles(mh, db);
						//debug_msg("-> exit case");
						break;
						
					case Protocol::COM_CREATE_ART:
						debug_msg("COM_CREATE_ART");
						create_article(mh, db);
						//debug_msg("exit case");
						break;
						
					case Protocol::COM_DELETE_ART:
						debug_msg("COM_DELETE_ART");
						delete_article(mh, db);
						//debug_msg("-> exit case");
						break;
						
					case Protocol::COM_GET_ART:
						debug_msg("COM_GET_ART");
						get_article(mh, db);
						//debug_msg("-> exit case");
						break;
				}


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

void debug_msg(const string msg) {
	if (debug_messages) {
		cout << msg << endl;
	}
}

void list_groups(MessageHandler& mh, database& db) {
	unsigned char com = mh.recv_code();
	if (com != Protocol::COM_END) {
		debug_msg("Connection should be thrown now");
	}
	mh.send_code(Protocol::ANS_LIST_NG);	
	auto groups = db.list_newsgroups();
	mh.send_int_p(groups.size());
	for (pair<unsigned,string> p : groups) {
		mh.send_int_p(p.first);
		mh.send_string_p(p.second);
	}
	mh.send_code(Protocol::ANS_END);
}

void create_newsgroup(MessageHandler& mh, database& db) {
	string name = mh.recv_string_p();
	unsigned char com = mh.recv_code();
	if (com != Protocol::COM_END) {
		debug_msg("Connection should be thrown now");
	}	
	unsigned char ans = db.create_newsgroup(name);	
	mh.send_code(Protocol::ANS_CREATE_NG);
	
	if (ans==Protocol::ANS_ACK)
		mh.send_code(ans);
	else if (ans==Protocol::ERR_NG_ALREADY_EXISTS) {
		mh.send_code(Protocol::ANS_NAK);
		mh.send_code(ans);
	} else {
		debug_msg("Connection should be thrown now");
	}	
	mh.send_code(Protocol::ANS_END);	
}

void delete_newsgroup(MessageHandler& mh, database& db) {
	unsigned id = mh.recv_int_p();
	debug_msg("id = " +to_string(id));	
	unsigned char com = mh.recv_code();
	
	if (com != Protocol::COM_END) {
		debug_msg("Connection should be thrown now");
	}
	unsigned char ans = db.delete_newsgroup(id);
	mh.send_code(Protocol::ANS_DELETE_NG);
	if (ans==Protocol::ANS_ACK)
		mh.send_code(ans);
	else if (ans==Protocol::ERR_NG_DOES_NOT_EXIST) {
		mh.send_code(Protocol::ANS_NAK);
		mh.send_code(ans);
	} else {
		debug_msg("Connection should be thrown now");
	}	
	mh.send_code(Protocol::ANS_END);
}

void list_articles(MessageHandler& mh, database& db) {
	unsigned id = mh.recv_int_p();
	unsigned char com = mh.recv_code();	
	if (com != Protocol::COM_END) {
		debug_msg("Connection should be thrown now");
	}
	auto ans = db.list_articles(id);
	mh.send_code(Protocol::ANS_LIST_ART);
	if (ans.first==Protocol::ANS_ACK) {	
		mh.send_code(ans.first);
		unsigned n = ans.second.size();
		mh.send_int_p(n);
		for (auto p : ans.second) {
			mh.send_int_p(p.first);
			mh.send_string_p(p.second);
		}
	} else if (ans.first==Protocol::ERR_NG_DOES_NOT_EXIST) {
		mh.send_code(Protocol::ANS_NAK);
		mh.send_code(ans.first);
	} else {
		debug_msg("Connection should be thrown now");
	}
	mh.send_code(Protocol::ANS_END);
}

void create_article(MessageHandler& mh, database& db) {
	unsigned id = mh.recv_int_p();
	article art;
	art.title = mh.recv_string_p();
	art.author = mh.recv_string_p();
	art.text = mh.recv_string_p();
	unsigned char com = mh.recv_code();	
	if (com != Protocol::COM_END) {
		debug_msg("Connection should be thrown now");
	}
	unsigned char ans = db.create_article(id, art);	
	mh.send_code(Protocol::ANS_CREATE_ART);
	if (ans == Protocol::ANS_ACK) {
		mh.send_code(ans);
	} else if (ans == Protocol::ERR_NG_DOES_NOT_EXIST) {
		mh.send_code(Protocol::ANS_NAK);
		mh.send_code(ans);
	} else {
		debug_msg("Connection should be thrown now");
	}	
	mh.send_code(Protocol::ANS_END);
}

void delete_article(MessageHandler& mh, database& db) {
	unsigned grp_id = mh.recv_int_p();
	unsigned art_id = mh.recv_int_p();
	unsigned char com = mh.recv_code();	
	if (com != Protocol::COM_END) {
		debug_msg("Connection should be thrown now");
	}
	unsigned char ans = db.delete_article(grp_id, art_id);
	mh.send_code(Protocol::ANS_DELETE_ART);
	if (ans == Protocol::ANS_ACK) 
		mh.send_code(ans);
	else if (ans==Protocol::ERR_NG_DOES_NOT_EXIST || ans==Protocol::ERR_ART_DOES_NOT_EXIST) {			
		mh.send_code(Protocol::ANS_NAK);
		mh.send_code(ans);
	} else {
		debug_msg("Connection should be thrown now");
	}
	mh.send_code(Protocol::ANS_END);	
}

void get_article(MessageHandler& mh, database& db) {
	unsigned grp_id = mh.recv_int_p();
	unsigned art_id = mh.recv_int_p();
	unsigned char com = mh.recv_code();	
	if (com != Protocol::COM_END) {
		debug_msg("Connection should be thrown now");
	}
	auto ans = db.get_article(grp_id, art_id);
	mh.send_code(Protocol::ANS_GET_ART);
	if (ans.first == Protocol::ANS_ACK) {
		mh.send_code(ans.first);
		mh.send_string_p(ans.second.title);
		mh.send_string_p(ans.second.author);
		mh.send_string_p(ans.second.text);
	} else if (ans.first==Protocol::ERR_NG_DOES_NOT_EXIST || ans.first==Protocol::ERR_ART_DOES_NOT_EXIST) {
		mh.send_code(Protocol::ANS_NAK);
		mh.send_code(ans.first);
	} else {
		debug_msg("Connection should be thrown now");
	}
	mh.send_code(Protocol::ANS_END);	
}


/*
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
*/

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
