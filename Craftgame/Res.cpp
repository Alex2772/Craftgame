#include "Res.h"

Resource::Resource(std::string domain, std::string path) {
	Resource::domain = domain;
	Resource::path = path;
	Resource::full = domain + ":" + path;
}
void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
}
Resource::Resource(std::string full) {
	Resource::full = full;
	std::vector<std::string> splt;
	split(full, ':', splt);
	if (splt.size()) {
		Resource::domain = splt[0];
		Resource::path = splt[1];
	}
	else {
		Resource::domain = "";
		Resource::path = "";
	}
}