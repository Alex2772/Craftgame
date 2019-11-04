#pragma once

#include <vector>
#include <chrono>
#include <string>

struct Section {
	std::string section;
	std::chrono::microseconds current;
	std::chrono::microseconds elapsed;
	std::chrono::microseconds prev;
};
struct Timer {
	std::string name;
	std::chrono::microseconds prev;
	std::chrono::microseconds current;
};

class Debugger {
private:
	std::vector<Section*> sections;
	std::vector<Timer*> timers;
public:
	Section* current = nullptr;
	Debugger();
	~Debugger();
	Section* getSection(std::string name);
	void beginSection(std::string name);
	Timer getTimer(std::string name);
	Timer* getTTimer(std::string name);
	void applyTimer(Timer& t);
	void resetTimers();
	void endSection(std::string name);
};

