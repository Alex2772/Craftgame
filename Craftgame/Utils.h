#pragma once

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "GuiScreen.h"
#include "ByteBuffer.h"

namespace Utils {
	glm::vec4 fromHex(size_t color);
	glm::vec3 convert(aiVector3D& v);
	std::string bytesFormat(size_t b);
	glm::vec2 convert(aiVector2D& v);
	glm::mat4 convert(const aiMatrix4x4& in_mat);
	template<typename T>
	std::string classname(T t)
	{
		std::string s = std::string(typeid(*t).name());
#ifdef WINDOWS // MSVC 2017 COMPILER
		return s.substr(6);
#elif defined(LINUX) // GCC LINUX COMPILER (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609
		return s;
#endif
	}
	void compress(const ByteBuffer& b, ByteBuffer& dst);
	void decompress(const ByteBuffer& b, ByteBuffer& dst);
	std::string pythonErrorString();
	void fileToString(std::string file, std::string& dst);
}
