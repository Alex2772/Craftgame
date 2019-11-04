#pragma once

#include <string>
#include "Mod.h"
#include <boost/python.hpp>

class PythonMod : public Mod {
private:
	std::string mFile;
public:
	PythonMod(boost::python::object file);

	virtual ~PythonMod() = default;
	boost::python::object handle;
	enum
	{
		HANDLE_MODULE,
		HANDLE_TYPE,
		HANDLE_MOD
	} state = HANDLE_MODULE;
};
