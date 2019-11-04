#include "Config.h"
#include "global.h"
#include "GameEngine.h"
#include "BinaryInputStream.h"
#include "BinaryOutputStream.h"
#include <sstream>
#include <memory>
#include "File.h"

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
					cfg.std::ifstream::read(data, length);
					n.data = data;
					n.size = int(length);
					nodes[s] = n;
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
	catch (std::bad_alloc e) { // TODO профиксить эту хуйню
		File f(file);
		f.remove();
		CGE::instance->logger->err("Cannot load confing " + file);
	}
}
ConfigFile::~ConfigFile() {
	for (tmap::iterator i = nodes.begin(); i != nodes.end(); i++) {
		Node n = i->second;
		delete n.r;
		delete n.data;
	}
	nodes.clear();
}
void ConfigFile::apply() {
	BinaryOutputStream cfg(file);
	for (tmap::iterator i = nodes.begin(); i != nodes.end(); i++) {
		cfg.writeTemplate<byte>(0x00);
		Node node = i->second;
		cfg.writeString(node.r->full);
		cfg.writeTemplate<int>(node.size);
		cfg.write(node.data, node.size);
	}
	cfg.close();
}