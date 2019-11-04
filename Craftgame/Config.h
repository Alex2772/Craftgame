#pragma once

#include <string>
#include "Res.h"
#include "CraftgameException.h"
#include <map>
#include <memory.h>

namespace Config {
	struct Node {
		_R* r;
		char* data = nullptr;
		int size;
	};

	class ConfigFile {
	private:
		std::string file;
	public:
		ConfigFile(std::string file);
		~ConfigFile();
		typedef std::map<std::string, Node> tmap;
		tmap nodes;
		template<typename T>
		T getProperty(_R name) {
			tmap::iterator i = nodes.find(name.full);
			if (i != nodes.end()) {
				return *reinterpret_cast<T*>((i->second.data));
			}
			throw std::runtime_error("Unknown property: " + name.full + " in config " + file);
		}
		std::string getPropertyString(_R name) {
			tmap::iterator i = nodes.find(name.full);
			if (i != nodes.end()) {
				return std::string(i->second.data, i->second.size);
			}
			throw std::runtime_error("Unknown property: " + name.full + " in config " + file);
		}
		template<typename T>
		T* getPropertyPointer(_R name) {
			tmap::iterator i = nodes.find(name.full);
			if (i != nodes.end()) {
				return reinterpret_cast<T*>(i->second.data);
			}
			return nullptr;
		}
		std::string* getPropertyPointer(_R name) {
			tmap::iterator i = nodes.find(name.full);
			if (i != nodes.end()) {
				std::string s(i->second.data, i->second.size);
				return &s;
			}
			return nullptr;
		}
		template<typename T>
		void setProperty(_R name, T value) {
			tmap::iterator i = nodes.find(name.full);
			if (i != nodes.end()) {
				if (sizeof(value) == i->second.size) {
					if (i->second.data) {
						memcpy(i->second.data, &value, sizeof(T));
					}
					else {
						i->second.data = reinterpret_cast<char*>(new T(value));
					}
					return;
				}
				CraftgameException e(this, "Conflicting data size while Config::setProperty");
				CrashInfo* ci = new CrashInfo(this);

				ci->addEntry("Config: ", file);
				ci->addEntry("Resource: ", name.full);
				ci->addEntry("New value: ", value);
				ci->addEntry("Data sizes: ", std::to_string(i->second.size) + "/" + std::to_string(sizeof(value)));
				e.addCI(ci);
				throw e;
			}
			/*
			for (int i = 0; i < nodes.size(); i++) {
				if (nodes[i].r->full == name.full) {
					if (!nodes[i].data)
						nodes[i].data = (char*)new T(value);
					else {
						memcpy(nodes[i].data, &value, sizeof(T));
					}
					return;
				}
			}*/
			Node node;
			node.r = new _R(name);
		
			node.data = reinterpret_cast<char*>(new T(value));
			node.size = sizeof(T);
			nodes[name.full] = node;
		}
		void setProperty(_R name, std::string value) {
			tmap::iterator i = nodes.find(name.full);
			if (i != nodes.end()) {
				if (i->second.data) {
					delete[] i->second.data;
				}
				i->second.data = new char[value.length()];
				i->second.size = int(value.length());
				memcpy(i->second.data, value.c_str(), value.length());
				return;
			}
			/*
			for (int i = 0; i < nodes.size(); i++) {
			if (nodes[i].r->full == name.full) {
			if (!nodes[i].data)
			nodes[i].data = (char*)new T(value);
			else {
			memcpy(nodes[i].data, &value, sizeof(T));
			}
			return;
			}
			}*/
			Node node;
			node.r = new _R(name);

			node.data = new char[value.length()];
			node.size = int(value.length());
			memcpy(node.data, value.c_str(), value.length());
			nodes[name.full] = node;
		}
		template<typename T>
		void bindDefaultValue(_R name, T value) {
			if (!getPropertyPointer<T>(name))
				setProperty(name, value);
		}
		void apply();
	};
}
