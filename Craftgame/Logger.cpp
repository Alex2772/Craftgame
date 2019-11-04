#pragma warning(disable: 4996)
#include "Logger.h"
#include <sstream>
#include <thread>
#include <ctime>
#include <iostream>
#include <sys/utime.h>

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
	std::unique_lock<std::mutex>(m);
	Logger::fos->close();
}
void Logger::log(std::string prefix, std::string message) {
	std::time_t t = std::time(NULL);
	std::tm* tm;
	tm = localtime(&t);
	char timebuf[64];
	std::strftime(timebuf, sizeof(timebuf), "%H:%M:%S", tm);

	std::stringstream ss;

	std::string threadName;
	std::map<std::thread::id, std::string>::iterator it = tNames.find(std::this_thread::get_id());
	if (it == tNames.end())
	{
		std::stringstream ss;
		ss << "thread #" << std::this_thread::get_id();
		threadName = ss.str();
	} else
	{
		threadName = it->second;
	}

	ss << "[" << timebuf << "] [" << threadName << "/" << prefix << "]: " << message << std::endl;

	std::unique_lock<std::mutex>(m);
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

/**
 * \brief Устанавливает имя текущего потока.
 */
void Logger::setThreadName(std::string name)
{
	std::unique_lock<std::mutex>(m);
	tNames[std::this_thread::get_id()] = name;
}

void Logger::setThreadName(std::thread::id id, std::string name)
{
	std::unique_lock<std::mutex>(m);
	tNames[id] = name;
}
