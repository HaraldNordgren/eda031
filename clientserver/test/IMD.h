#include "database.h"

#include <map>
#include <string>

class IMD : public database {
public:
	std::map<unsigned, std::string> list_newsgroups();
	constant create_newsgroup(std::string name);
private:
	std::map<unsigned, std::string> groups;
	int counter = 0;
};
