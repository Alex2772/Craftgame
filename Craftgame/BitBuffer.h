#pragma once

#include <vector>

class BitBuffer {
private:
	std::vector<unsigned char> _data;
	size_t pos = 0;
	
	unsigned char pow(unsigned char b, unsigned char p) {
		for (unsigned char i = 1; i < p; i++) {
			b *= b;
		}
		return b;
	}
public:
	BitBuffer() {

	}
	void write(unsigned char* data, size_t size) {
		for (size_t i = 0; i < size; i++) {
			_data.push_back(data[i]);
		}
	}
	bool read() {
		unsigned char b = _data[pos / 8];
		bool a = (b & pow(pos == 1 ? 1 : 2, pos % 8)) ? 1 : 0;
		pos++;
		return a;
	}
	size_t seekg() {
		return pos;
	}
	size_t read(unsigned char size) {
		size_t value = 0;
		for (unsigned char i = 0; i < size; i++) {
			size_t t = read();
			t <<= i;
			value |= t;
		}
		return value;
	}
};