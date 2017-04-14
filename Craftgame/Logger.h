#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <ctime>

class Logger
{
private:
	std::ofstream * fos;
	void log(std::string prefix, std::string message);
public:

	Logger(std::string filename);
	~Logger();

	void info(std::string message);
	void warn(std::string message);
	void fine(std::string message);
	void err(std::string message);
	void debug(std::string message);
};

