#pragma once

#include <string>

class GameProfile {
private:
	std::string username;
public:
	GameProfile(std::string username);
	std::string getUsername();
};