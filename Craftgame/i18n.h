#pragma once
#pragma warning(disable: 4996)
#include <string>
#include <map>
#include <fstream>
#include "Parsing.h"
#include <boost/variant.hpp>
class my_visitor : public boost::static_visitor<std::string>
{
public:
	template<typename T>
	std::string operator()(T i) const
	{
		return std::to_string(i);
	}

	std::string operator()(std::string str) const
	{
		return str;
	}
};
class my_visitor2 : public boost::static_visitor<std::string>
{
public:
	template<typename T>
	std::string operator()(T i) const
	{
		return std::to_string(i);
	}

	std::string operator()(std::string str) const
	{
		return std::string("\"") + str + "\"";
	}
};
typedef boost::variant<std::string, int, float, double> vr;
class spair
{
public:
	std::string name;
	vr value;
	spair(std::string n, vr n2):
	name(n),
	value(n2)
	{
		
	}
};
class Locale {
protected:
	typedef std::map<std::string, vr> smap;
	typedef std::map<std::string, std::string> mtype;
	mtype data;

	template<typename Arg>
	void rec(smap& m, Arg&& a)
	{
		m[a.name] = a.value;
	}

	void pr(bool& ename, smap& vmap, std::string& result, std::string& mname);

	template<typename Arg, typename... Args>
	void rec(smap& m, Arg&& a, Args&&... s)
	{
		rec(m, a);
		rec(m, s...);
	}
	std::string parseBrackets(std::string& format, smap& vmap);
	std::string parse(std::string& format, smap& vmap);
	Locale* parent;

	std::string englishName;
	std::string name;
public:
	Locale(std::string locale) {
		std::ifstream fis("res/lang/" + locale + ".lang");
		if (!fis.good()) {
			fis = std::ifstream("res/lang/en_US.lang");
		}
		std::string s;
		while (std::getline(fis, s)) {
			if (Parsing::startsWith(s, "#!")) {
				std::vector<std::string> l = Parsing::splitString(s.substr(2), ":");
				if (l.size() >= 1) {
					englishName = l[0];
				}
				if (l.size() >= 2) {
					name = l[1];
				}
				if (l.size() >= 3) {
					parent = new Locale(l[2]);
				}
			}
			else {
				std::vector<std::string> l = Parsing::splitString(s, "=");
				if (l.size() == 2) {
					data[l[0]] = l[1];
				}
			}
		}
		fis.close();
	}
	~Locale() {
		delete parent;
	}

	template <typename... Args>
	std::string localize(std::string target, Args&&... a) {
		mtype::iterator f = data.find(target);
		smap vmap;
		rec(vmap, std::forward<Args>(a)...);
		if (f == data.end()) {
			if (parent)
				return parent->localize(target, a...);
			std::stringstream ss;
			ss << target << "[";
			for (smap::iterator i = vmap.begin(); i != vmap.end(); i++) {
				if (i != vmap.begin()) {
					ss << ", ";
				}
				ss << i->first << "=" << boost::apply_visitor(my_visitor2(), i->second);
			}
			ss << "]";
			return ss.str();
		}
		std::string format = f->second;
		std::string result;
		bool ename = false;
		return parse(format, vmap);
	}
	std::string localize(std::string target) {
		mtype::iterator f = data.find(target);
		if (f == data.end()) {
			if (parent)
				return parent->localize(target);
			return target;
		}
		std::string format = f->second;
		smap vmap;
		return parse(format, vmap);
	}
};
