#include "Utils.h"
#ifndef SERVER
#include "CustomShader.h"
#include "GameEngine.h"

CustomShader::CustomShader(std::string vertexCode, std::string fragmentCode):
	StaticShader(vertexCode, fragmentCode)
{
}

void CustomShader::compile()
{
	if (program != 0) {
		unload();
	}
	if (onCompile)
		onCompile();
	vertex = createShader(fvertex, GL_VERTEX_SHADER);
	fragment = createShader(ffragment, GL_FRAGMENT_SHADER);
	if (!fgeometry.empty())
	{
		geometry = createShader(fgeometry, GL_GEOMETRY_SHADER);
	}
	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	if (geometry)
		glAttachShader(program, geometry);
	if (!mAttributes.empty()) {
		size_t co = 0;
		for (std::string& s : mAttributes) {
			glBindAttribLocation(program, co++, s.c_str());
		}
	}
	if (!mAttributes.empty()) {
		link();
	}
}
extern void compileStaticShader(Shader* s);
extern void compileSkeletonShader(Shader* s);
void CustomShader::shader(Type t, const std::string& name, const std::string& code, std::map<std::string, std::string> uniforms, std::function<void(boost::shared_ptr<Shader>)> render)
{
	if (!(CGE::instance->renderer->renderType & GameRenderer::RENDER_SHADOW))
	{
		boost::shared_ptr<Shader> shader;
		if (!(shader = CGE::instance->customShaders[std::string("cs_") + name]).get())
		{
			static std::string vertex;
			if (vertex.empty())
			{
				switch (t)
				{
				case STATIC:
					Utils::fileToString("res/glsl/staticV.glsl", vertex);
					break;
				case SKELETON:
					Utils::fileToString("res/glsl/skeletonV.glsl", vertex);
					break;
				}
			}
			std::string fragment;
			Utils::fileToString("res/glsl/staticF.glsl", fragment);
			{
				std::string uni;
				for (std::pair<std::string, std::string> p : uniforms)
				{
					uni += std::string("uniform ") + p.second + " " + p.first + ";\n";
				}
				fragment.insert(fragment.find('\n') + 1, uni);
			}
			fragment += "\nvec3 custom_code(vec3 normal) {";
			fragment += code;
			fragment += "}";

			shader = boost::make_shared<CustomShader>(vertex, fragment);
			CGE::instance->customShaders[std::string("cs_") + name] = shader;
			shader->defines["CGE_CUSTOM"] = 1;
			switch (t)
			{
			case STATIC:
				compileStaticShader(shader.get());
				break;
			case SKELETON:
				compileSkeletonShader(shader.get());
				break;
			}
		}
		switch (t)
		{
		case STATIC: {
			StaticShader* s = CGE::instance->defaultStaticShader;
			CGE::instance->defaultStaticShader = boost::dynamic_pointer_cast<StaticShader>(shader).get();
			shader->start();
			shader->loadMatrix("projection", CGE::instance->projection);
			render(shader);
			CGE::instance->defaultStaticShader = s;
			CGE::instance->defaultStaticShader->start();
			CGE::instance->defaultStaticShader->use();
			break;
		}
		case SKELETON: {
			StaticShader* s = CGE::instance->skeletonShader;
			CGE::instance->skeletonShader = boost::dynamic_pointer_cast<StaticShader>(shader).get();
			shader->start();
			shader->loadMatrix("projection", CGE::instance->projection);
			render(shader);
			CGE::instance->skeletonShader = s;
			CGE::instance->defaultStaticShader->start();
			CGE::instance->defaultStaticShader->use();
			break;
		}
		}
	}
}

#endif
