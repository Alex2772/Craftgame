#include "Utils.h"
#include "global.h"
#include <zlib.h>
#include <boost/python.hpp>
#include <fstream>

glm::vec4 Utils::fromHex(size_t color) {
	byte r = color >> 24;
	byte g = color >> 16;
	byte b = color >> 8;
	byte a = color;
	glm::vec4 v = glm::vec4(float(r) / 255.f, float(g) / 255.f, float(b) / 255.f, float(a) / 255.f);
	return v;
}
glm::mat4 Utils::convert(const aiMatrix4x4& in_mat) {
	glm::mat4 tmp;
	tmp[0][0] = in_mat.a1;
	tmp[1][0] = in_mat.a2;
	tmp[2][0] = in_mat.a3;
	tmp[3][0] = in_mat.a4;

	tmp[0][1] = in_mat.b1;
	tmp[1][1] = in_mat.b2;
	tmp[2][1] = in_mat.b3;
	tmp[3][1] = in_mat.b4;

	tmp[0][2] = in_mat.c1;
	tmp[1][2] = in_mat.c2;
	tmp[2][2] = in_mat.c3;
	tmp[3][2] = in_mat.c4;

	tmp[0][3] = in_mat.d1;
	tmp[1][3] = in_mat.d2;
	tmp[2][3] = in_mat.d3;
	tmp[3][3] = in_mat.d4;
	return tmp;
}
glm::vec2 Utils::convert(aiVector2D& v) {
	return glm::vec2(v.x, v.y);
}
glm::vec3 Utils::convert(aiVector3D& v) {
	return glm::vec3(v.x, v.y, v.z);
}

std::string Utils::bytesFormat(size_t b)
{
	unsigned char postfix_level = 0;
	while (b > 2048)
	{
		b = b / 1024;
		postfix_level++;
	}
	std::string n = std::to_string(b);
	switch (postfix_level)
	{
	case 0:
		return n + " b";
	case 1:
		return n + " kb";
	case 2:
		return n + " mb";
	case 3:
		return n + " gb";
	case 4:
		return n + " tb";
	case 5:
		return n + " pb";
	default:
		return n + " ??";
	}
}

void Utils::compress(const ByteBuffer& b, ByteBuffer& dst)
{
	dst.reserve(b.available() + 12);
	uLong len = b.available() + 12;
	int r = compress2(reinterpret_cast<Bytef*>(const_cast<char*>(dst.getCurrentPosAddress())), &len, reinterpret_cast<Bytef*>(const_cast<char*>(b.getCurrentPosAddress())), b.available(), Z_BEST_COMPRESSION);
	if (r != Z_OK)
	{
		throw CraftgameException(std::string("zlib compress error ") + std::to_string(r));
	}
	dst.setSize(dst.size() + len);
	dst.seekg(dst.size());
}

void Utils::decompress(const ByteBuffer& b, ByteBuffer& dst)
{
	for (size_t i = 2;; i++) {
		dst.reserve(b.available() * i);
		uLong len = dst.reserved();
		int r = uncompress(reinterpret_cast<Bytef*>(const_cast<char*>(dst.get())), &len, reinterpret_cast<Bytef*>(const_cast<char*>(b.getCurrentPosAddress())), b.available());
		if (r == Z_BUF_ERROR)
		{
			continue;
		} else if (r != Z_OK)
		{
			throw CraftgameException(std::string("zlib decompress error ") + std::to_string(r));
		}
		dst.setSize(dst.size() + len);
		return;
	}
}

using namespace boost::python;
using namespace std;
std::string Utils::pythonErrorString()
{
	PyObject *ptype, *pvalue, *ptraceback;
	PyErr_Fetch(&ptype, &pvalue, &ptraceback);

	handle<> hType(ptype);
	object extype(hType);
	std::string str;
	if (ptraceback) {
		handle<> hTraceback(ptraceback);
		object traceback(hTraceback);
		//Extract line number (top entry of call stack)
		// if you want to extract another levels of call stack
		// also process traceback.attr("tb_next") recurently
		long lineno = extract<long>(traceback.attr("tb_lineno"));
		string filename = extract<string>(traceback.attr("tb_frame").attr("f_code").attr("co_filename"));
		string funcname = extract<string>(traceback.attr("tb_frame").attr("f_code").attr("co_name"));
		str += funcname + " (" + filename + ":" + std::to_string(lineno) + ")";

	}
	if (pvalue) {
		if (!str.empty())
			str += ": ";
		handle<> hTraceback(pvalue);
		object traceback(hTraceback);
		boost::python::str s(traceback);
		str += boost::python::extract<std::string>(s);
	}
	return str;
}

void Utils::fileToString(std::string file, std::string& dst)
{
	std::ifstream fis(file, std::ios::in);
	if (fis.is_open())
	{
		std::string Line = "";
		while (getline(fis, Line)) {
			dst += Line + "\n";
		}
		fis.close();
	}
}
