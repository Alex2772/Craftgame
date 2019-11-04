#include "Parsing.h"

#include <sstream>


std::vector<std::string> Parsing::splitString(const std::string& s, std::string delim) {
	std::vector<std::string> v;
	std::stringstream ss;
	ss.str(s);
	std::string line;
	while (Parsing::getLine(ss, line, delim))
		v.push_back(line);
	return v;
}
float CmdLineArg::toFloat()
{
	switch (type)
	{
	case T_FLOAT:
		return get<float>();
	case T_INTEGER:
		return get<int>();
	}
	return 0;
}
int CmdLineArg::toInt()
{
	switch (type)
	{
	case T_FLOAT:
		return get<float>();
	case T_INTEGER:
		return get<int>();
	}
	return 0;
}

void CmdLineArg::clear()
{
	if (data && dataSize) {
		delete[] data;
	}
	data = nullptr;
	dataSize = 0;
}

int Parsing::getLine(std::istream& istream, std::string& ss, std::string delim) {
	std::stringstream str;
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
bool Parsing::endsWith(std::string& s, std::string e) {
	if (s.length() >= e.length()) {
		return (0 == s.compare(s.length() - e.length(), e.length(), e));
	}
	else {
		return false;
	}
}
bool Parsing::startsWith(std::string& s, std::string e) {
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
				int data = boost::lexical_cast<int>(next);
				arg.data = (char*)new int(data);
				arg.dataSize = sizeof(int);
				arg.type = T_INTEGER;
			}
			catch (...) {
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

void Parsing::parseArgsServer(std::vector<CmdLineArg>& vect, Command*& cmd, std::string& s)
{
	
	std::vector<std::string> a = Parsing::splitString(s, " ");
	if (a.size()) {
		if (cmd = CGE::instance->commandRegistry->findCommandByName(a[0])) {
			for (size_t i = 1; i < a.size(); i++) {
				std::string elem = a[i];
				CmdLineArg arg;
				arg.name = new char[1];
				arg.name[0] = '\0';
				arg.data = new char[1];
				arg.data[0] = '\0';
				arg.dataSize = 1;
				arg.type = T_FLAG;
				if (Parsing::startsWith(elem, "-")) {
					arg.name = elem.substr(1);
					if (a.size() > i + 1) {
						elem = a[i + 1];
						i++;
						if (Parsing::startsWith(elem, "-")) {
							vect.push_back(arg);
							continue;
						}
					}
					else {
						vect.push_back(arg);
						continue;
					}
				}
				delete[] arg.data;
				try {
					if (elem.find(".") != std::string::npos)
					{
						float data = stof(elem);
						arg.type = T_FLOAT;
						arg.dataSize = sizeof(data);
						arg.data = new char[sizeof(data)];
						memcpy(arg.data, reinterpret_cast<char*>(&data), sizeof(arg.data));
					}
					else {
						int data = stoi(elem);
						arg.type = T_INTEGER;
						arg.dataSize = sizeof(data);
						arg.data = new char[sizeof(data)];
						memcpy(arg.data, reinterpret_cast<char*>(&data), sizeof(arg.data));
					}
				}
				catch (std::invalid_argument e) {
					arg.dataSize = elem.size() + 1;
					arg.type = T_STRING;
					arg.data = new char[elem.size() + 1];
					memcpy(arg.data, elem.c_str(), elem.size() + 1);
				}
				vect.push_back(arg);
			}
		}
	}
	if (Parsing::endsWith(s, " "))
	{
		CmdLineArg arg;
		arg.name = new char[1];
		arg.name[0] = '\0';
		arg.data = new char[1];
		arg.data[0] = '\0';
		arg.dataSize = 1;
		arg.type = T_STRING;
		vect.push_back(arg);
	}
}

std::string CmdLineArg::toString()
{
	switch (type)
	{
	case T_INTEGER:
		return std::to_string(get<int>());
	case T_BOOLEAN:
		return std::to_string(get<bool>());
	case T_FLOAT:
		return std::to_string(get<float>());
	case T_STRING:
		return data;
	default:
		return "";
	}
}
