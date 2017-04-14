#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class Resource {
public:
	std::string domain;
	std::string path;
	std::string full;

	Resource(std::string domain, std::string path);
	Resource(std::string full);
};

typedef Resource _R;


inline bool operator==(const _R& r1, const _R& r2) {
	return r1.full == r2.full;
}