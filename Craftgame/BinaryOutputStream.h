#pragma once
#include <fstream>

using namespace std;

class BinaryOutputStream :
	public ofstream {
public:
	BinaryOutputStream(std::string file) :
		ofstream(file, std::ios::binary) {

	}
	void writeInt(int i) {
		ofstream::write((char*)&i, sizeof(int));
	}
	void writeFloat(float i) {
		ofstream::write((char*)&i, sizeof(float));
	}
	void writeString(std::string s) {
		writeInt((int)s.length());
		ofstream::write((char*)s.c_str(), s.length());
	}
	void writeShort(short s) {
		ofstream::write((char*)&s, sizeof(short));
	}
	void writeByte(unsigned char c) {
		ofstream::write((char*)&c, 1);
	}
	template<class T>
	void writeTemplate(T t) {
		ofstream::write((char*)&t, sizeof(t));
	}
};