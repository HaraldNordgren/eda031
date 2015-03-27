#include <map>
#include <string>

using constant = unsigned char; 

struct article {
	std::string title, author, text;
};

class database {
public:
	virtual std::map<unsigned, std::string> list_newsgroups() = 0;
	virtual constant create_newsgroup(std::string name) = 0;
	/*virtual constant delete_newsgroup(unsigned id) = 0;
	virtual pair<constant, std::map<unsigned, std::string>> list_articles() = 0;
	virtual constant create_article(unsigned id, article) = 0;
	virtual constant delete_article(unsigned gid, unsigned aid) = 0;
	virtual pair<constant, article> get_article(unsigned gid, unsigned aid) = 0;*/
private:
};
