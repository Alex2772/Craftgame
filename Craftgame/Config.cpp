#include "Config.h"
#include "global.h"
#include "GameEngine.h"
#include "BinaryInputStream.h"
#include "BinaryOutputStream.h"
#include <sstream>
#include <memory>

using namespace Config;

ConfigFile::ConfigFile(std::string file) {
	ConfigFile::file = file;
	CGE::instance->logger->info("Loading config " + file);
	try {
		BinaryInputStream cfg(file);
		while (cfg.good()) {
			byte header = cfg.read<byte>();
			if (header == 0x00) {
				Node n;
				std::string s = cfg.readString();
				if (s.size()) {
					n.r = new _R(s);
					int length = cfg.read<int>();
					char* data = new char[length];
					cfg.ifstream::read(data, length);
					n.data = data;
					n.size = int(length);
					nodes.push_back(n);
				}
			}
			else {
				std::stringstream ss;
				ss << std::hex << int(header);
				CGE::instance->logger->err("Found unknown directive " + ss.str());
			}
		}
		cfg.close();
	}
	catch ( CraftgameException e) {
		CGE::instance->logger->err("Cannot load config " + file);
	}
}
ConfigFile::~ConfigFile() {
	for (size_t i = 0; i < nodes.size(); i++) {
		Node n = nodes[i];
		delete n.r;
		delete n.data;
	}
	nodes.clear();
}
void ConfigFile::apply() {
	BinaryOutputStream cfg(file);
	for (int i = 0; i < nodes.size(); i++) {
		cfg.writeTemplate<byte>(0x00);
		Node node = nodes[i];
		cfg.writeString(node.r->full);
		cfg.writeTemplate<int>(node.size);
		cfg.write(node.data, node.size);
	}
	cfg.close();
}