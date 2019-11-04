#pragma once

#include <string>

class GameProfile {
private:
	std::string username;
public:
	GameProfile(std::string username);
	const std::string& getUsername() const;
};