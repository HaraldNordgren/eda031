#include "disk_database.h"
#include "protocol.h"

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

using namespace std;

static const string ROOT_PATH = "../../db/";
static const string COUNTER_PATH = ROOT_PATH + "counter";
static const string DELIMITER = ":";

disk_database::disk_database() {
	DIR* root = opendir(ROOT_PATH.c_str());
	if (root != nullptr) {
		closedir(root);
		return;
	}

	mkdir(ROOT_PATH.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);	
	counter.open(COUNTER_PATH, fstream::out | fstream::trunc);
	counter << "0";
	counter.close();
}

vector<pair<unsigned, string>> disk_database::list_newsgroups() {
	DIR* root = opendir(ROOT_PATH.c_str());
	if (root == nullptr) {
		cerr << "Couldn't locate database!" << endl;
		exit(1);
	}

	vector<pair<unsigned, string>> result;
	struct dirent* folder;

	while ((folder = readdir(root)) != NULL) {
		string folder_name = folder->d_name;
		auto pos = folder_name.find(DELIMITER);
		if (pos != string::npos) {
			string id = folder_name.substr(0,pos);
			string name = folder_name.substr(pos+1);
			result.push_back({stoi(id), name});
		}
	}
	
	closedir(root);
	sort(result.begin(), result.end());
	
	return result;
}

constant disk_database::create_newsgroup(string name) {
	DIR* root = opendir(ROOT_PATH.c_str());
	if (root == nullptr) {
		cerr << "Couldn't locate database!" << endl;
		exit(1);
	}

	struct dirent* folder;
	while ((folder = readdir(root)) != NULL) {
		string folder_name = folder->d_name;
		auto pos = folder_name.find(DELIMITER);
		if (pos != string::npos && folder_name.substr(pos+1) == name) {
			closedir(root);
			return Protocol::ERR_NG_ALREADY_EXISTS;
		}
	}

	closedir(root);
	
	unsigned count;
	counter.open(COUNTER_PATH);
	counter >> count;
	const string NEWSGROUP_PATH = ROOT_PATH + to_string(++count)
		+ DELIMITER + name;	
	mkdir(NEWSGROUP_PATH.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);

	counter.clear();	
	counter.seekp(0);
	counter << count;
	counter.close();
	
	return Protocol::ANS_ACK;
}

constant disk_database::delete_newsgroup(unsigned id) {
	/*auto it = newsgroups.find(id);
	if (it == newsgroups.end()) {
		return Protocol::ERR_NG_DOES_NOT_EXIST;
	}
	
	newsgroups.erase(it);*/
	return Protocol::ANS_ACK;
}

pair<constant, vector<pair<unsigned, string>>> disk_database::list_articles(unsigned id) {
	vector<pair<unsigned, string>> result;
	
	/*auto it = newsgroups.find(id);
	if (it == newsgroups.end()) {
		return {Protocol::ERR_NG_DOES_NOT_EXIST, result};
	}

	for (auto entry : it->second.articles) {
		result.push_back({entry.first, entry.second.title});
	}*/
	return {Protocol::ANS_ACK, result};
}


constant disk_database::create_article(unsigned id, article art) {
	/*auto it = newsgroups.find(id);
	if (it == newsgroups.end()) {
		return Protocol::ERR_NG_DOES_NOT_EXIST;
	}

	newsgroup ng = it->second;
	ng.articles.insert({++ng.counter, art});*/
	return Protocol::ANS_ACK;
}

constant disk_database::delete_article(unsigned group_id, unsigned article_id) {
	/*auto it_group = newsgroups.find(group_id);
	if (it_group == newsgroups.end()) {
		return Protocol::ERR_NG_DOES_NOT_EXIST;
	}
	
	auto articles = it_group->second.articles;
	auto it_articles = articles.find(article_id);
	if (it_articles == articles.end()) {
		return Protocol::ERR_ART_DOES_NOT_EXIST;
	}

	articles.erase(it_articles);*/
	return Protocol::ANS_ACK;
}

pair<constant, article> disk_database::get_article(unsigned group_id, unsigned article_id) {
	/*auto it_group = newsgroups.find(group_id);
	if (it_group == newsgroups.end()) {
		article art;
		return {Protocol::ERR_NG_DOES_NOT_EXIST, art};
	}
	
	auto articles = it_group->second.articles;
	auto it_articles = articles.find(article_id);
	if (it_articles == articles.end()) {
		article art;
		return {Protocol::ERR_ART_DOES_NOT_EXIST, art};
	}*/

	article art;
	return {Protocol::ANS_ACK, art};
}
