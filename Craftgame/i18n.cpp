#include "i18n.h"
std::string Locale::parseBrackets(std::string& format, smap& vmap)
{
	std::string result;
	std::vector<std::string> strArray;
	std::string curr;
	for (std::string::iterator i = format.begin(); i != format.end(); i++)
	{
		if (*i == '|')
		{
			if (i != format.begin() && *(i - 1) != '\\')
			{
				strArray.push_back(curr);
				curr.clear();
				continue;
			}
		}
		curr += *i;
	}
	strArray.push_back(curr);
	if (strArray.size() <= 1)
	{
		return format;
	}
	if (Parsing::startsWith(strArray[0], "%") && Parsing::endsWith(strArray[0], "%"))
	{
		size_t count = stoi(boost::apply_visitor(my_visitor(), vmap[strArray[0].substr(1, strArray[0].size() - 2)]));
		count++;
		return parse(strArray[count >= strArray.size() ? strArray.size() - 1 : count], vmap);
	} else
	{
		return parse(strArray[rand() % strArray.size()], vmap);
	}
	return format;
}
std::string Locale::parse(std::string& format, smap& vmap)
{
	std::string result;
	try {
		std::string mname;
		bool ename = false;

		for (std::string::iterator i = format.begin(); i != format.end(); i++)
		{
			if (ename && *i == '%')
			{
				pr(ename, vmap, result, mname);
			}
			else {
				if (*i == '%')
				{
					if (i != format.begin() && *(i - 1) != '\\')
					{
						ename = true;
						continue;
					}
				}
				if (ename)
				{
					mname += *i;
				}
				else if (*i == '[')
				{
					std::string::iterator start = ++i;
					std::string::iterator end = format.begin();

					for (; i != format.end() && *i != ']'; i++)
					{
					}
					if (i == format.end() || *i != ']')
					{
						throw;
					}
					end = i;
					std::string fsr(start, end);
					result += parseBrackets(fsr, vmap);
				}
				else
				{
					result += *i;
				}
			}
		}
	}
	catch (...)
	{
		return format;
	}
	return result;
}
void Locale::pr(bool& ename, smap& vmap, std::string& result, std::string& mname)
{
	if (ename)
	{
		ename = false;
		smap::iterator f = vmap.find(mname);
		if (f == vmap.end()) {
			result += std::string("%") + mname;
		}
		else
		{
			vr& var = f->second;
			std::string v = boost::apply_visitor(my_visitor(), var);
			result += v;
		}

		mname.clear();
	}
}