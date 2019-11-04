#pragma once
#include <string>
#include "EntityPlayerMP.h"
#include <queue>
#include <ctime>
#include <chrono>

struct ChatMessage
{
	std::string message;
	long long timestamp;
};

class Chat
{
public:
	typedef std::deque<ChatMessage> mqueue;
	mqueue chatQueue;
	std::deque<std::string> clientQueue;
	bool full = false;
	int height = 0;
	int scroll = 0;
	Chat();
	void broadcast(std::string s);
	void onPlayerChatMessage(EntityPlayerMP* from, std::string message);
	void addChat(std::string m);
	void clear();

	void sendMessage(std::string message);
};
