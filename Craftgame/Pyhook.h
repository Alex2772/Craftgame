#pragma once
#include <boost/python.hpp>


std::string hook_pudge()
{
	srand(std::time(nullptr));
	std::vector<std::string> phrases = {
		"Alex2772 is pudgepicker --",
		"Come to Pudge!",
		"Oy, fresh meat!",
		"Fresh meat!!",
		"Look who's coming for dinner!"
	};
	return phrases[rand() % phrases.size()];
}

BOOST_PYTHON_MODULE(hook)
{
	boost::python::def("pudge", &hook_pudge);
}
