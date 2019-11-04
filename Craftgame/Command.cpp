#include "Command.h"

Arg::Arg(std::string ___n, std::string __n, ArgType t, bool _n) :
	name(___n),
	type(t),
	need(_n),
	n(__n){

}

std::string Command::constructCommandUsage()
{
	std::vector<Arg> a = this->getArgTypes();
	std::string s = getName();
	for (size_t i = 0; i < a.size(); i++) {
		s += " ";
		Arg g = a[i];
		if (g.need) {
			s += "<";
		}
		else {
			s += "[";
		}
		if (g.n.size()) {
			s += "-" + g.n + " ";
		}
		s += g.name + ":";
		if (g.type & T_NUMBER) {
			s += "number";
		} else {
			switch (g.type) {
			case ArgType::T_STRING:
				s += "string";
				break;
			case ArgType::T_BOOLEAN:
				s += "boolean";
				break;
			case ArgType::T_INTEGER:
				s += "integer";
				break;
			case ArgType::T_FLOAT:
				s += "float";
				break;
			}
		}
		if (g.need) {
			s += ">";
		}
		else {
			s += "]";
		}
	}
	return s;
}

std::string & Command::getName()
{
	return name;
}
