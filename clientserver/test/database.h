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
	virtual std::vector<std::pair<unsigned, std::string>> list_newsgroups() const = 0;
	virtual constant create_newsgroup(const std::string) = 0;
	virtual constant delete_newsgroup(const unsigned) = 0;
	virtual std::pair<constant, std::vector<std::pair<unsigned, std::string>>>
		list_articles(const unsigned) const = 0;
	virtual constant create_article(const unsigned, const article) = 0;
	virtual constant delete_article(const unsigned, const unsigned) = 0;
	virtual std::pair<constant, article>
		get_article(const unsigned, const unsigned) const = 0;
};

#endif
