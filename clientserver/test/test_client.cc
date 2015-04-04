

#include "connection.h"
#include "connectionclosedexception.h"
#include "protocol.h"
#include "messagehandler.h"
#include "database.h"


#include <vector>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cstdlib>
#include <map>
#include <memory>

using namespace std;
struct state {
	unsigned current_group = 0;
	map<string, unsigned> group_nametoindex;
	map<string, unsigned> article_nametoindex;
};

/* database communication functions */
vector<pair<unsigned, string>> listNewsgroups(MessageHandler& mh);
unsigned char createNewsgroup(MessageHandler& mh);
unsigned char deleteNewsgroup(MessageHandler& mh, const unsigned id);
pair<unsigned char, vector<pair<unsigned, string>>> listArticles(MessageHandler& mh, const unsigned id);
unsigned char createArticle(MessageHandler& mh, const article art);
unsigned char deleteArticle(MessageHandler& mh, const unsigned grp_id, const unsigned art_id);

/* utility functions */
pair<string, string> parse_cmdline(string line);
bool isnumber(const string& s);

/* console functions */
void console_list_newsgroups(MessageHandler& mh, state& s, bool print);
void console_create_newsgroup(MessageHandler& mh, state& s, const string& input);
void console_delete_newsgroup(MessageHandler& mh, state& s, const string& input);
void console_list_articles(MessageHandler& mh, state& s, const unsigned id);
void console_create_article(MessageHandler& mh, state& s);
void console_delete_article(MessageHandler& mh, state& s);
void update_console(MessageHandler& mh, const string& command, const string& input, state& s);



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
	
	string s;
	state console_state;
	unsigned current_group = 0;
	auto temp =  shared_ptr<Connection>(&conn);		/* Detta går nog göra bättre. 	*/
	MessageHandler mh(temp);						/* Funkar dock (hittils).		*/ 
	
	
	/* ----------------------------- LOOP ----------------------------- */
	while (getline(cin, s)) {
		auto p = parse_cmdline(s);		
		try {
			update_console(mh, p.first, p.second, console_state);		
		} catch (ConnectionClosedException&) {
			cout << " no reply from server. Exiting." << endl;
			exit(1);
		}
	}
}

/* ------------------------------- DATABASE COMMUNICATION FUNCTIONS ------------------------------- */

vector<pair<unsigned, string>> listNewsgroups(MessageHandler& mh) {

	mh.send_code(Protocol::COM_LIST_NG);
	mh.send_code(Protocol::COM_END);
	unsigned char code = mh.recv_code();
	if (code != Protocol::ANS_LIST_NG) {
		/* error */
	}
	unsigned n = mh.recv_int_p();
	vector<pair<unsigned, string>> newsgroups;
	for (size_t i = 0; i != n; ++i) {
		unsigned id = mh.recv_int_p();
		string name = mh.recv_string_p();
		newsgroups.push_back(make_pair(id,name));
	}
	code = mh.recv_code();
	if (code != Protocol::ANS_END) {
		/* error */
	}
	return newsgroups;
}

unsigned char createNewsgroup(MessageHandler& mh, const string name) {
	mh.send_code(Protocol::COM_CREATE_NG);
	mh.send_string_p(name);
	mh.send_code(Protocol::COM_END);
	unsigned char code = mh.recv_code();
	if (code != Protocol::ANS_CREATE_NG) {
		/* error */
	}
	code = mh.recv_code();
	if (code == Protocol::ANS_NAK) {
		code = mh.recv_code();
	} else if (code != Protocol::ANS_ACK) {
		/* error */
	}
	auto retval = code;
	code = mh.recv_code();
	if (code != Protocol::ANS_END) {
		/* error */
	}
	return retval;
}

unsigned char deleteNewsgroup(MessageHandler& mh, const unsigned id) {
	mh.send_code(Protocol::COM_DELETE_NG);
	mh.send_int_p(id);
	mh.send_code(Protocol::COM_END);
	unsigned char code = mh.recv_code();
	if (code != Protocol::ANS_DELETE_NG) {
		/* error */
	}
	code = mh.recv_code();
	if (code == Protocol::ANS_NAK) {
		code = mh.recv_code();
	} else if (code != Protocol::ANS_ACK) {
		/* error */
	}
	auto retval = code;
	code = mh.recv_code();
	if (code != Protocol::ANS_END) {
		/* error */
	}
	return retval;	
}

pair<unsigned char, vector<pair<unsigned, string>>> listArticles(MessageHandler& mh, const unsigned id) {
	mh.send_code(Protocol::COM_LIST_ART);
	mh.send_int_p(id);
	mh.send_code(Protocol::COM_END);
	unsigned char code = mh.recv_code();
	if (code != Protocol::ANS_LIST_ART) {
		/* error */
	}
	
	pair<unsigned char, vector<pair<unsigned,string>>> retval;
	
	code = mh.recv_code();
	if (code == Protocol::ANS_ACK) {
		retval.first = code;
		unsigned n = mh.recv_int_p();
		for (size_t i = 0; i<n; ++i) {	
			unsigned art_id = mh.recv_int_p();
			string art_name = mh.recv_string_p();	
			retval.second.push_back(make_pair(art_id, art_name));
		}
	} else if (code == Protocol::ANS_NAK) {
		code = mh.recv_code();
		retval.first = code;
	} else {
		/* error */
	}	
	code = mh.recv_code();
	return retval;
}

unsigned char createArticle(MessageHandler& mh, const unsigned id, const article art) {
	mh.send_code(Protocol::COM_CREATE_ART);
	mh.send_int_p(id);
	mh.send_string_p(art.title);
	mh.send_string_p(art.author);
	mh.send_string_p(art.text);
	mh.send_code(Protocol::COM_END);
	
	unsigned char code = mh.recv_code();
	if (code != Protocol::ANS_CREATE_ART) {
		/* error */
	}
	code = mh.recv_code();
	if (code == Protocol::ANS_NAK) {
		code = mh.recv_code();
	} else if (code != Protocol::ANS_ACK) {
		/* error */
	}
	unsigned char retval = code;
	code = mh.recv_code();
	return retval;
}

unsigned char deleteArticle(MessageHandler& mh, const unsigned grp_id, const unsigned art_id) {
	mh.send_code(Protocol::COM_DELETE_ART);
	mh.send_int_p(grp_id);
	mh.send_int_p(art_id);
	mh.send_code(Protocol::COM_END);
	
	unsigned char code = mh.recv_code();
	if (code != Protocol::ANS_DELETE_ART) {
		/* error */
	}
	code = mh.recv_code();
	unsigned char retval;
	
	if (code == Protocol::ANS_NAK) {
		code = mh.recv_code();
	} else if (code != Protocol::ANS_ACK) {
		/* error */
	}
	retval = code;
	code = mh.recv_code();
	return retval;
}
	
/* ------------------------------- UTILITY FUNCTIONS ------------------------------- */

pair<string, string> parse_cmdline(string line) {
	size_t pos_space = line.find_first_of(" ");
	string command =  line.substr(0,pos_space);	
	string input("");	
	if (pos_space != string::npos) {
		input = line.substr(pos_space);		
		size_t pos1 = input.find_first_not_of(" ");	
		if (pos1 == string::npos) return make_pair(command, "");
		size_t pos2 = input.find_last_not_of(" ");
		input = input.substr(pos1, pos2-pos1+1);
	}
	return make_pair(command,input);
}

bool isnumber(const string& s) {
	return s.find_first_not_of("0123456789") == string::npos;
}


/* ------------------------------- CONSOLE FUNCTIONS ------------------------------- */

void console_list_newsgroups(MessageHandler& mh, state& s, bool print=true) {
	s.current_group = 0; // =no group
	s.group_nametoindex = map<string, unsigned>();
	auto groups = listNewsgroups(mh);
	for (auto p : groups) {
		if (print) cout << p.first << ". " << p.second << endl;
		s.group_nametoindex.insert(make_pair(p.second, p.first));
	}
}

void console_create_newsgroup(MessageHandler& mh, state& s) {	
	/* input name */
	string name;
	cout << "newsgroup name> ";
	getline(cin, name);	
	auto pos = name.find_first_not_of(" ");
	if (pos == string::npos) {
		cout << "<invalid input>" << endl;
		return;
	}
	/* create group */
	auto code = createNewsgroup(mh, name);
	if (code==Protocol::ERR_NG_ALREADY_EXISTS) {
		cout << "<Group already exists>" << endl;
	} else {
		/* refresh state variable s */
		console_list_newsgroups(mh, s, false);			
		s.current_group = s.group_nametoindex.at(name);
	}
}

void console_delete_newsgroup(MessageHandler& mh, state& s) {


	string input;
	cout << "delete newsgroup> ";
	getline(cin, input);
	
	unsigned id;
	if (isnumber(input)) {
		id = stoi(input);
	} else {
		auto it = s.group_nametoindex.find(input);
		if (it == s.group_nametoindex.end()) {
			cout << "<Group does not exist>" << endl;
			return;
		}
		id = s.group_nametoindex.at(input);
	}	
	auto code = deleteNewsgroup(mh, id);
	if (code==Protocol::ERR_NG_DOES_NOT_EXIST) {
		cout << "<Group does not exist>" << endl;
	} else {
		console_list_newsgroups(mh, s, false);
		s.current_group = 0;
	}
}

void console_list_articles(MessageHandler& mh, state& s, const string name) {
	auto it = s.group_nametoindex.find(name);
	if (it == s.group_nametoindex.end()) {
		cout << "<Group does not exist>" << endl;
		return;
	}
	unsigned id = s.group_nametoindex.at(name);
	auto p = listArticles(mh, id);
	if (p.first == Protocol::ERR_NG_DOES_NOT_EXIST) {
		cout << "Group does not exist" << endl;
	} else {
		s.article_nametoindex = map<string, unsigned>();
		for (auto art : p.second) {
			cout << art.first << ". " << art.second << endl;
			s.article_nametoindex.insert(make_pair(art.second,art.first));	
		}
		s.current_group = id;
	}
}

void console_create_article(MessageHandler& mh, state& s) {	
	/* any group selected? */
	if (s.current_group == 0) {
		cout << "<Select a newsgroup first>" << endl;
		return;
	}
	/* construct article */
	article art;
	
	/* input title */
	cout << "article title> ";
	getline(cin, art.title);	
	auto pos = art.title.find_first_not_of(" ");
	if (pos == string::npos) {
		cout << "<invalid input>" << endl;
		return;
	}	
	/* input author */
	cout << "article author> ";
	getline(cin, art.author);
	pos = art.author.find_first_not_of(" ");
	if (pos == string::npos) {
		cout << "<invalid input>" << endl;
		return;
	}	
	/* input text */
	cout << "article text> ";
	getline(cin, art.text);	
	pos = art.text.find_first_not_of(" ");
	if (pos == string::npos) {
		cout << "<invalid input>" << endl;
		return;
	}
	/* send article to database */
	unsigned char code = createArticle(mh, s.current_group, art);	
}

void console_delete_article(MessageHandler& mh, state& s) {
	/* any group selected? */
	if (s.current_group == 0) {
		cout << "<Select a newsgroup first>" << endl;
		return;
	}	
	/* get input */
	string input;
	cout << "delete article> ";
	getline(cin, input);
	unsigned id;
	if (isnumber(input)) {
		id = stoi(input);
	} else {
		auto it = s.article_nametoindex.find(input);
		if (it==s.article_nametoindex.end()) {
			cout << "<no such article>" << endl;
			return;
		}
		id = s.article_nametoindex.at(input);
	}
	/* send delete instructions to database	*/
	unsigned char code = deleteArticle(mh, s.current_group, id);
	if (code == Protocol::ERR_NG_DOES_NOT_EXIST) {
		cout << "<newsgroup does not exist>" << endl;
	}
	if (code == Protocol::ERR_ART_DOES_NOT_EXIST) {
		cout << "<article does not exist>" << endl;
	}	
}

void update_console(MessageHandler& mh, const string& command, const string& input, state& s) {
	if (command == "list") {
		if (input == "") {	
			console_list_newsgroups(mh, s);			
		} else {			
			console_list_articles(mh, s, input);
		}
	} else if (command == "create") {
		if (input == "") {
			cout << "<Please specify what to create>" << endl;
		} else if (input=="newsgroup") {			
			console_create_newsgroup(mh, s);
		} else if (input=="article") {	
			console_create_article(mh, s);
		} else {
			cout << "Valid inputs are: 'create newsgroup' or 'create article'" << endl;
		}
	} else if (command == "delete") {
		if (input == "") {
			cout << "<Please specify what to delete>" << endl;
		} else if (input == "newsgroup") { 
			console_delete_newsgroup(mh, s);
		} else if (input == "article") {
			console_delete_article(mh, s);
		} else {			
			/* */	
		}
	} else if (command=="create_art") {
		
	} else if (command=="exit") {
		cout << "<exiting>" << endl;
		exit(1);
	} else {
		cout << "<command not valid>" << endl;
		cout << "These are the valid commands:\n-" << endl;
		/* */
	}
	cout << "news";
	if (s.current_group != 0) {
		cout << "/group_" <<s.current_group;
	}
	cout << "> ";
}



