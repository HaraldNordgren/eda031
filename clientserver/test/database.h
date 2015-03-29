#ifndef DB
#define DB

#include <map>
#include <string>
#include <vector>

using constant = unsigned char; 

struct article {
	std::string title, author, text;
};

struct newsgroup {
	std::string name;
	std::map<unsigned, article> articles;
	unsigned counter = 0;
};

class database {
public:
	virtual std::vector<std::pair<unsigned, std::string>> list_newsgroups() = 0;
	virtual constant create_newsgroup(std::string) = 0;
	virtual constant delete_newsgroup(unsigned) = 0;
	virtual std::pair<constant, std::vector<std::pair<unsigned, std::string>>>
		list_articles(unsigned) = 0;
	virtual constant create_article(unsigned, article) = 0;
	virtual constant delete_article(unsigned, unsigned) = 0;
	virtual std::pair<constant, article> get_article(unsigned, unsigned) = 0;
};

#endif
