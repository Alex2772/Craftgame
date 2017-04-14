#pragma warning(disable: 4996)
#include "Logger.h"
#include <sstream>

Logger::Logger(std::string filename) {
	Logger::fos = new std::ofstream(filename, std::ios::app);
	if (Logger::fos->is_open()) {
		Logger::fine("Opened file " + filename);
	}
	else {
		Logger::warn("Could not open log file " + filename);
	}
}
Logger::~Logger() {
	Logger::fos->close();
}
void Logger::log(std::string prefix, std::string message) {
	std::time_t t = std::time(NULL);
	std::tm tm;
	tm = *localtime(&t);
	char timebuf[10];
	std::strftime(timebuf, sizeof(timebuf), "%H:%M:%S", &tm);

	std::stringstream ss;

	ss << "[" << timebuf << "] [" << prefix << "]: " << message << std::endl;

	std::cout << ss.str();
	(*Logger::fos) << ss.str();
}

void Logger::info(std::string message) {
	log("INFO", message);
}
void Logger::warn(std::string message) {
	log("WARN", message);
}
void Logger::fine(std::string message) {
	log("FINE", message);
}
void Logger::err(std::string message) {
	log("ERR", message);
}
void Logger::debug(std::string message) {
	log("DEBUG", message);
}
