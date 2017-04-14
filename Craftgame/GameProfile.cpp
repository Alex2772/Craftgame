#include "GameProfile.h"

GameProfile::GameProfile(std::string u) :
	username(u) {

}
std::string GameProfile::getUsername() {
	return username;
}