#pragma once

#include <map>
#include <GL/glew.h>
#include <GL\GL.h>

class SkinManager {
private:
	typedef std::map<std::string, size_t*> Skins;
	Skins skins;
public:
	SkinManager();
	~SkinManager();
	size_t* getSkin(std::string name);
};