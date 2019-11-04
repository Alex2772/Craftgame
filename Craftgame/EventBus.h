#pragma once

#pragma warning(disable: 4996)

#include "Res.h"
#include <map>
#include <boost/python.hpp>
#include <boost/python/str.hpp>
#include "Gui.h"
//typedef std::map<std::string, boost::python::pointer_wrapper_base*> ObjectMap;

using namespace boost;

class Event
{
public:
	Resource name;
	Event(Resource r):
		name(r)
	{
		
	}
	virtual ~Event() = default;
};
class PythonEvent: public Event
{
private:
	bool cancelled = false;
public:
	std::map<std::string, boost::python::object> dict;
	void cancel()
	{
		cancelled = true;
	}
	bool isCancelled()
	{
		return cancelled;
	}
	PythonEvent(Resource r) :
		Event(r)
	{

	}
	template<typename T>
	PythonEvent* param(std::string s, T t)
	{
		dict[s] = python::object(python::ptr(t));
		return this;
	}
	PythonEvent* param(std::string s, std::string t);
	boost::python::object& Py_GetAttr(std::string str)
	{
		if (dict.find(str) == dict.end())
		{
			char buffer[64];
			sprintf(buffer, "PythonEvent instance has no attribute '%s'", str.c_str());
			PyErr_SetString(PyExc_AttributeError, buffer);
			throw boost::python::error_already_set();
		}
		return dict[str];
	}
};

class EventBus
{
private:
	static bool process(Event* e);
public:
	static bool pre(Event* e);
	static bool ev(Event* e);
	static void post(Event* e);
};
