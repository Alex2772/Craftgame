#pragma once
#include <fstream>
#include <map>
#include <thread>
#include <mutex>

class Logger
{
private:
	std::ofstream * fos;
	void log(std::string prefix, std::string message);
	std::map<std::thread::id, std::string> tNames;
	std::mutex m;
public:

	Logger(std::string filename);
	~Logger();

	void info(std::string message);
	void warn(std::string message);
	void fine(std::string message);
	void err(std::string message);
	void debug(std::string message);
	void setThreadName(std::string name);
	void setThreadName(std::thread::id id, std::string name);
};
