#pragma once
#include "StaticShader.h"
#include "Material.h"
#include <boost/smart_ptr/shared_ptr.hpp>

class CustomShader : public StaticShader {
public:
	Material* material = nullptr;
	CustomShader(std::string vertexCode, std::string fragmentCode);
	virtual void compile();
	enum Type
	{
		STATIC,
		SKELETON
	};
	static void shader(Type t, const std::string& name, const std::string& code, std::map<std::string, std::string> uniforms, std::function<void(boost::shared_ptr<Shader>)> render);
};
