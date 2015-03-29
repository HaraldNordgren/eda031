#include "database.h"

#include <map>
#include <string>
#include <vector>

class IMD : public database {
public:
	std::vector<std::pair<unsigned, std::string>> list_newsgroups();
	constant create_newsgroup(std::string);
	constant delete_newsgroup(unsigned);
	std::pair<constant, std::vector<std::pair<unsigned, std::string>>>
		list_articles(unsigned);
	constant create_article(unsigned, article);
	constant delete_article(unsigned, unsigned);
	std::pair<constant, article> get_article(unsigned, unsigned);
private:
	std::map<unsigned, newsgroup> newsgroups;
	unsigned counter = 0;
};