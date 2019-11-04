#include "GameProfile.h"

GameProfile::GameProfile(std::string u) :
	username(u) {

}
const std::string& GameProfile::getUsername() const
{
	return username;
}
