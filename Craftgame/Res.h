#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>
#include <map>

class Resource {
public:
	class IStreamProvider
	{
	public:
		virtual std::shared_ptr<std::istream> openStream(std::string) = 0;
	};

	std::string domain;
	std::string path;
	std::string full;

	Resource(std::string domain, std::string path);
	Resource(std::string full);
	Resource(const char* full);
	std::shared_ptr<std::istream> open();

	static void registerIStreamProvider(std::string ns, IStreamProvider* isp);
private:
	static std::map<std::string, IStreamProvider*> _kek;
};

typedef Resource _R;


inline bool operator==(const _R& r1, const _R& r2) {
	return r1.full == r2.full;
}
inline bool operator<(const _R& r1, const _R& r2) {
	return r1.full < r2.full;
}
inline bool operator>(const _R& r1, const _R& r2) {
	return r1.full > r2.full;
}