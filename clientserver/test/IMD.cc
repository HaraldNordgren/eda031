#include "IMD.h"
#include "protocol.h"

#include <string>
#include <map>

using namespace std;

std::map<unsigned, string> IMD::list_newsgroups() {
	return groups;
}

constant IMD::create_newsgroup(string name) {
	groups.insert({++counter, name});
	return Protocol::ANS_ACK;
}

