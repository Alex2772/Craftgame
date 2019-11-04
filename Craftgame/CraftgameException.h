#pragma once

#include <exception>
#include <vector>
#include <sstream>
#include <string>
#include <utility>
#include <typeinfo>


class CrashInfo {
public:
	std::vector<std::pair<std::string, std::string>> entries;
	std::string cb;
	template<class C>
	CrashInfo(C)
	{
		cb = typeid(C).name();
	}

	template<typename T>
	void addEntry(std::string name, T data) {
		std::stringstream s;
		s << "<";
		s << typeid(data).name();
		s << "> ";
		s << data;
		entries.push_back(make_pair(name, s.str()));
	}
};

class CraftgameException: public std::runtime_error {
private:
	std::string cb;
	std::vector<CrashInfo*> ci;
	std::string temps;
public:
	CraftgameException(const char* s) :
		std::runtime_error(s)
	{

	}
	CraftgameException(std::string s) :
		std::runtime_error(s)
	{

	}
	template<class CausedBy>
	CraftgameException(CausedBy _cb):
		std::runtime_error("")
		{
		cb = typeid(_cb).name();
	}
	template<class CausedBy>
	CraftgameException(CausedBy _cb, std::string s):
		std::runtime_error(s)
		{
		cb = typeid(_cb).name();
	}
	CraftgameException(const CraftgameException& r):
		std::runtime_error(r.std::runtime_error::what())
	{
		for (CrashInfo* c : r.ci) {
			ci.push_back(new CrashInfo(*c));
		}
	}
	~CraftgameException() {
		for (size_t i = 0; i < ci.size(); i++) {
			delete (ci[i]);
		}
	}
	void addCI(CrashInfo* _ci) {
		ci.push_back(_ci);
	}
	virtual char const* what()
	{
		std::stringstream ss;
		ss << std::runtime_error::what();
		if (!cb.empty())
			ss << std::endl << "Caused by: " << cb;
		if (!ci.empty()) {
			ss << std::endl << "Additional info:" << std::endl;
			for (size_t i = 0; i < ci.size(); i++) {
				ss << "###############################" << std::endl;
				ss << "# Class: " << ci[i]->cb << std::endl;
				for (size_t j = 0; j < ci[i]->entries.size(); j++) {
					ss << "# " << ci[i]->entries[j].first << ": " << ci[i]->entries[j].second << std::endl;
				}
				ss << "###############################";
			}
		}
		temps = ss.str();
		return temps.c_str();
	}
};
