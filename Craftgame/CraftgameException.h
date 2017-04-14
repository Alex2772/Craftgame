#pragma once

#include <exception>

class CraftgameException: public std::runtime_error {
public:
	CraftgameException():
		std::runtime_error(""){
	}
	CraftgameException(std::string s):
		std::runtime_error(s) {
	}
};
