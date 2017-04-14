#pragma once

#include <string>
#include "Res.h"

namespace Config {
	struct Node {
		_R* r;
		char* data;
		int size;
	};

	class ConfigFile {
	private:
		std::string file;
	public:
		ConfigFile(std::string file);
		~ConfigFile();
		std::vector<Node> nodes;
		template<typename T>
		T getProperty(_R& name) {
			for (int i = 0; i < nodes.size(); i++) {
				if (nodes[i].r->full == name.full) {
					return (T)(*(nodes[i].data));
				}
			}
			throw std::runtime_error("Unknown property: " + name.full + " in config " + file);
		}
		template<typename T>
		T* getPropertyPointer(_R& name) {
			for (int i = 0; i < nodes.size(); i++) {
				if (nodes[i].r->full == name.full) {
					return (T*)(nodes[i].data);
				}
			}
			return nullptr;
		}

		template<typename T>
		void setProperty(_R& name, T value) {
			for (int i = 0; i < nodes.size(); i++) {
				if (nodes[i].r->full == name.full) {
					if (nodes[i].data)
						delete nodes[i].data;
					nodes[i].data = (char*)new T(value);
					return;
				}
			}
			Node node;
			node.r = new _R(name);
			node.data = (char*)new T(value);
			node.size = sizeof(T);
			nodes.push_back(node);
		}

		template<typename T>
		void bindDefaultValue(_R& name, T value) {
			if (!getPropertyPointer<T>(name))
				setProperty(name, value);
		}
		void apply();
	};
}