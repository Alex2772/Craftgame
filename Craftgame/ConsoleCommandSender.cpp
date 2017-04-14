#include "ConsoleCommandSender.h"
#include "GameEngine.h"

ConsoleCommandSender::ConsoleCommandSender() {

}
void ConsoleCommandSender::sendMessage(std::string s) {
	CGE::instance->logger->info(s);
}