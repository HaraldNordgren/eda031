#include "database.h"

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>

class disk_database : public database {
public:
	disk_database(std::string);

	std::vector<std::pair<unsigned, std::string>> list_newsgroups() const;
	constant create_newsgroup(const std::string);
	constant delete_newsgroup(const unsigned);
	std::pair<constant, std::map<unsigned, std::string>>
		list_articles(const unsigned) const;
	constant create_article(const unsigned, const article);
	constant delete_article(const unsigned, const unsigned);
	std::pair<constant, article>
		get_article(const unsigned, const unsigned) const;
private:
	std::string DATABASE_ROOT = "./database/";
};
