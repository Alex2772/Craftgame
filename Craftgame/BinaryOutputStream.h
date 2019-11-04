#pragma once
#include <fstream>



class BinaryOutputStream :
	public std::ofstream {
public:
	BinaryOutputStream(std::string file) :
		std::ofstream(file, std::ios::binary) {

	}
	void writeInt(int i) {
		std::ofstream::write((char*)&i, sizeof(int));
	}
	void writeFloat(float i) {
		std::ofstream::write((char*)&i, sizeof(float));
	}
	void writeString(std::string s) {
		writeInt((int)s.length());
		std::ofstream::write((char*)s.c_str(), s.length());
	}
	void writeShort(short s) {
		std::ofstream::write((char*)&s, sizeof(short));
	}
	void writeByte(unsigned char c) {
		std::ofstream::write((char*)&c, 1);
	}
	template<class T>
	void writeTemplate(T t) {
		std::ofstream::write((char*)&t, sizeof(t));
	}
};