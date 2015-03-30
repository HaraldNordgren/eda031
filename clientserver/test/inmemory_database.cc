#include "inmemory_database.h"
#include "protocol.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

vector<pair<unsigned, string>> inmemory_database::list_newsgroups() const {
	vector<pair<unsigned, string>> result;
	for (auto entry : newsgroups) {
		result.push_back({entry.first, entry.second.name});
	}
	return result;
}

constant inmemory_database::create_newsgroup(const string name) {
	auto it = newsgroups.begin();
	while (it != newsgroups.end() && it->second.name != name) {
		++it;
	}
	
	if (it != newsgroups.end()) {
		return Protocol::ERR_NG_ALREADY_EXISTS;
	}

	newsgroup ng;
	ng.name = name;	
	newsgroups.insert({++counter, ng});
	return Protocol::ANS_ACK;
}

constant inmemory_database::delete_newsgroup(const unsigned id) {
	auto it = newsgroups.find(id);
	if (it == newsgroups.end()) {
		return Protocol::ERR_NG_DOES_NOT_EXIST;
	}
	
	newsgroups.erase(it);
	return Protocol::ANS_ACK;
}

pair<constant, map<unsigned, string>> inmemory_database::list_articles(const unsigned id) const {
	map<unsigned, string> result;
	
	auto it = newsgroups.find(id);
	if (it == newsgroups.end()) {
		return {Protocol::ERR_NG_DOES_NOT_EXIST, result};
	}

	for (auto entry : it->second.articles) {
		result.insert({entry.first, entry.second.title});
	}

	return {Protocol::ANS_ACK, result};
}


constant inmemory_database::create_article(const unsigned id, const article art) {
	auto it = newsgroups.find(id);
	if (it == newsgroups.end()) {
		return Protocol::ERR_NG_DOES_NOT_EXIST;
	}

	newsgroup& ng = it->second;
	ng.articles.insert({++ng.counter, art});

	return Protocol::ANS_ACK;
}

constant inmemory_database::delete_article(const unsigned group_id, const unsigned article_id) {
	auto it_group = newsgroups.find(group_id);
	if (it_group == newsgroups.end()) {
		return Protocol::ERR_NG_DOES_NOT_EXIST;
	}
	
	auto articles = it_group->second.articles;
	auto it_articles = articles.find(article_id);
	if (it_articles == articles.end()) {
		return Protocol::ERR_ART_DOES_NOT_EXIST;
	}

	articles.erase(it_articles);
	return Protocol::ANS_ACK;
}

pair<constant, article> inmemory_database::get_article(const unsigned group_id, const unsigned article_id) const {
	auto it_group = newsgroups.find(group_id);
	if (it_group == newsgroups.end()) {
		article art;
		return {Protocol::ERR_NG_DOES_NOT_EXIST, art};
	}
	
	auto articles = it_group->second.articles;
	auto it_articles = articles.find(article_id);
	if (it_articles == articles.end()) {
		article art;
		return {Protocol::ERR_ART_DOES_NOT_EXIST, art};
	}

	return {Protocol::ANS_ACK, it_articles->second};
}
