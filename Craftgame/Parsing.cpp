#include "Parsing.h"

#include <sstream>


vector<string> Parsing::splitString(string& s, string delim) {
	vector<string> v;
	stringstream ss;
	ss.str(s);
	string line;
	while (Parsing::getLine(ss, line, delim))
		v.push_back(line);
	return v;
}
int Parsing::getLine(std::istream& istream, string& ss, string delim) {
	stringstream str;
	int match = 0;
	for (int i = 0;; i++) {
		char s;
		if (istream.readsome(&s, 1) == 0) {
			ss = str.str();
			if (ss.length() > 0)
				break;
			return 0;
		}
		if (delim.length() > match && delim[match] == s) {
			match++;
		}
		else {
			str << s;
			match = 0;
		}
		if (match == delim.length()) {
			ss = str.str();
			return 1;
		}

	}
	ss = str.str();
	return 1;
}
bool Parsing::endsWith(string& s, string e) {
	if (s.length() >= e.length()) {
		return (0 == s.compare(s.length() - e.length(), e.length(), e));
	}
	else {
		return false;
	}
}
bool Parsing::startsWith(string& s, string e) {
	if (s.length() >= e.length()) {
		return (0 == s.compare(0, e.length(), e));
	}
	else {
		return false;
	}
}

#include "GameEngine.h"

std::vector<CmdLineArg> Parsing::parseArgsFromString(std::string& s) {
	std::vector<CmdLineArg> vect;
	std::vector<std::string> a = splitString(s, " ");
	
	for (size_t i = 0; i < a.size(); i++) {
		std::string elem = a[i];
		if (startsWith(elem, "--") && elem.size() > 2) {
			if (a.size() - 1 == i) {
				WARN(std::string("Argument ") + elem + " hasn't data, ignoring");
			}
			CmdLineArg arg;
			std::string next = a[++i];
			arg.name = elem.substr(2);
			try {
				int data = stoi(next);
				arg.data = (char*)new int(data);
				arg.dataSize = sizeof(int);
				arg.type = T_INTEGER;
			}
			catch (std::invalid_argument e) {
				arg.dataSize = next.size() + 1;
				arg.data = new char[arg.dataSize];
				arg.type = T_STRING;
				memcpy(arg.data, next.c_str(), arg.dataSize);
			}
			vect.push_back(arg);
		}
		else if (startsWith(elem, "-") && elem.size() > 1) {
			CmdLineArg arg;
			arg.name = elem.substr(1);
			arg.type = T_FLAG;
			vect.push_back(arg);
		}
		else {
			WARN(std::string("Argument ") + elem + " cannot be parsed, ignoring");
		}
	}
	return vect;
}