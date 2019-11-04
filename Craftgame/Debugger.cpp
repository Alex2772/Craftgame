#include "Debugger.h"

Debugger::Debugger()
{
}

Debugger::~Debugger()
{
	for (size_t i = 0; i < sections.size(); i++) {
		delete sections[i];
	}for (size_t i = 0; i < timers.size(); i++) {
		delete timers[i];
	}
	sections.clear();
	timers.clear();
}

Section * Debugger::getSection(std::string name)
{
	for (size_t i = 0; i < sections.size(); i++) {
		Section* s = sections[i];
		if (s->section == name)
			return s;
	}
	Section* s = new Section;
	s->section = name;
	s->elapsed = (std::chrono::microseconds)0;
	s->current = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
	sections.push_back(s);
	return s;
}

void Debugger::beginSection(std::string name)
{
	if (current) {
		current->elapsed += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()) - current->current;
	}
	Section* s = getSection(name);
	s->current = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
	current = s;
}

Timer Debugger::getTimer(std::string name)
{
	Timer t;
	t.name = name;
	t.current = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch());
	return t;
}

Timer * Debugger::getTTimer(std::string name)
{
	for (size_t i = 0; i < timers.size(); i++) {
		if (timers[i]->name == name) {
			return timers[i];
		}
	}
	Timer* j = new Timer;
	j->name = name;
	j->current = (std::chrono::microseconds)0;
	timers.push_back(j);
	return j;
}

void Debugger::applyTimer(Timer & t)
{
	t.current = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()) - t.current;
	getTTimer(t.name)->current += t.current;
}

void Debugger::resetTimers()
{
	for (size_t i = 0; i < timers.size(); i++) {
		timers[i]->prev = timers[i]->current;
		timers[i]->current = (std::chrono::microseconds)0;
	}
	for (size_t i = 0; i < sections.size(); i++) {
		sections[i]->prev = sections[i]->elapsed;
		sections[i]->elapsed = (std::chrono::microseconds)0;
	}
}

void Debugger::endSection(std::string name)
{
	Section* s = getSection(name);
	s->elapsed += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()) - s->current;
	current = nullptr;
}
