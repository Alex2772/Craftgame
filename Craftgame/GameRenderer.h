#pragma once
#include "Model.h"
#include "AdvancedModel.h"
#include "CFont.h"
#include <functional>
#include "Block.h"
#include "Frustum.h"

enum Align {
	LEFT,
	CENTER,
	RIGHT
};
enum TextStyle {
	SIMPLE,
	SHADOW
};

struct FontStyle
{
	TextStyle ts = TextStyle::SIMPLE;
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
	long size = FONT_SIZE;
	CFont* font = nullptr;
};

class GameRenderer {
private:
	bool gui3d = false;
public:
	GameRenderer();
	void drawIngameGui();
	ViewFrustum frustum;
	Material* material = nullptr;
	glm::vec4 color = glm::vec4(1, 1, 1, 1);
	int texturing = 1;

	enum RenderType
	{
		RENDER_FIRST = 8,
		RENDER_LAST = 16,
		RENDER_DEFAULT = 1,
		RENDER_SHADOW = 2
	};
	int renderType = RENDER_DEFAULT;

	enum RenderFlags
	{
		RF_SSAO = 1
	};
	int renderFlags = RF_SSAO;


	void renderModel(SimpleModel* model, char type = 0);
	void renderModel(AdvancedModel* model, bool materialBinding = true);
	void renderModelAssimp(AdvancedAssimpModel* model, std::map<std::string, SkeletalAnimation>* anims, bool materialBinding = true);
	void renderModelAssimp(AdvancedAssimpModel* model, std::map<std::string, SkeletalAnimation>* anims, std::map<std::string, glm::mat4>& transforms, bool materialBinding = true);
	void renderModelAssimpOnly(AdvancedAssimpModel* model, std::string name, std::map<std::string, SkeletalAnimation>* anims, std::map<std::string, glm::mat4>& t, bool materialBinding);
	void drawRect(int x, int y, int width, int height);
	void drawBorder(int x, int y, int width, int height, int w);
	void drawRect(int x, int y, int width, int height, float u, float v);
	void drawRect(float x, float y, float width, float height, float u, float v, float u2, float v2);
	void drawRectNUV(int x, int y, int width, int height); // Python
	void drawRectUV(int x, int y, int width, int height, float u, float v); // Python
	void drawRectUVUV(int x, int y, int width, int height, float u, float v, float u2, float v2); // Python
	void drawTextureAtlas(int x, int y, int width, int height);
	void blur(int x, int y, int width, int height, int radius = 13);
	void blur(int x, int y, int width, int height, std::function<void()> func, int radius = 13);
	FontStyle& drawString(int x, int y, std::string text, Align align = Align::LEFT, TextStyle style = TextStyle::SIMPLE, glm::vec4 color = glm::vec4(1, 1, 1, 1), long size = FONT_SIZE, CFont* font = nullptr, bool formatting = false);
	FontStyle& drawString(int x, int y, std::string text, Align align, FontStyle& fs, bool formatting = false);
	void drawShadow(int x, int y, int width, int height, glm::vec4 c, int spr, int radius);
	void renderBlock(Block* b, glm::mat4 tr);
	void renderBlockIcon(Block* b);
	void renderBlockIcon(Block* b, int x, int y);
	void setColor(glm::vec4) const;
	void setTexturingMode(int mode);
	void setMaskingMode(int mode) const;
	void setTransform(glm::mat4 t);
	void setGui3D(bool f);

	void enablePerspective() const;
	void disablePerspective() const;

	glm::mat4 transform = glm::mat4(1.0);
	int viewportX;
	int viewportY;
	int viewportW;
	int viewportH;
	bool noAT = false;
	void changeViewport(int x, int y, int width, int height);

	void renderChunks(byte);
	virtual void render(byte);
};