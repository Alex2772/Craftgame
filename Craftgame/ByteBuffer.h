#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <memory.h>

using namespace std;

class ByteBuffer {
private:
	std::vector<char> buf;
	size_t currentPos = 0;
public:
	ByteBuffer();
	template<class T>
	void put(T& data) {
		char* d = reinterpret_cast<char*>(&data);
		buf.insert(buf.end(), d, d + sizeof(T));
	}
	template<class T>
	void get(T& data) {
		memcpy((char*)&data, buf.data() + currentPos, sizeof(T));
		currentPos += sizeof(T);
	}
	void put(const char* c, const size_t size);
	void get(char* c, const size_t size);
	const char* get();
	size_t size();
};
ByteBuffer& operator<<(ByteBuffer& b, std::string& data);
ByteBuffer& operator >> (ByteBuffer& b, std::string& data);
template<class T>
ByteBuffer& operator<<(ByteBuffer& b, T& data) {
	b.put(data);
	return b;
}
template<class T>
ByteBuffer& operator>>(ByteBuffer& b, T& data) {
	b.get(data);
	return b;
}
