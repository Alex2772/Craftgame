#include "Exceptions.h"
#ifndef SERVER
#include "GuiScreenScreenshots.h"
#include "GameEngine.h"
#include "AnimationFade.h"
#include "GuiButton.h"
#include "Socket.h"


void GuiScreenScreenshots::loadImage(std::string url)
{
	CGE::instance->threadPool.runAsync([&, url]()
	{
		blur = true;
		Image* im = CGE::instance->imageLoader->load(_R(url));
		CGE::instance->runOnSecondThread([&, url, im]()
		{
			Texture texture;
			CGE::instance->textureManager->get(_R("gui/temp"), texture);
			GLuint s = CGE::instance->textureManager->loadTexture(im, _R("gui/temp"), true);
			if (t.load())
			{
				glDeleteTextures(1, &texture.texture);
				delete texture.image;
			}
			glFinish();
			{
				img = im;
				t = s;
				blur = false;
				std::vector<std::string> splt = Parsing::splitString(url, "/");
				if (!splt.empty()) {
					std::string img = splt[splt.size() - 1];
					CGE::instance->uiThread.push([&, img]()
					{
						imageName = img;
					});
				}
			}
		});
	});
}
void GuiScreenScreenshots::imagesPage(std::string url)
{
	try {
		std::stringstream s;
		http_get(url, &s);
		std::string page = s.str();

		size_t search = 0;
		while (1) {
			search = page.find("href=\"", search + 1);
			if (search == std::string::npos)
				break;
			search += 6;
			size_t n = page.find("\"", search);
			std::string u = page.substr(search, n - search);
			if (Parsing::endsWith(u, ".png") || Parsing::endsWith(u, ".jpg")) {
				if (Parsing::startsWith(u, "http")) {
					urlsTT.push_back(u);
				}
				else
				{
					if (!Parsing::endsWith(url, "/"))
					{
						url += "/";
					}
					urlsTT.push_back(url + u);
				}
			}
			search = n;
		}
		search = 0;
		while (1) {
			search = page.find("src=\"", search + 1);
			if (search == std::string::npos)
				break;
			search += 5;
			size_t n = page.find("\"", search);
			std::string u = page.substr(search, n - search);
			if (Parsing::endsWith(u, ".png") || Parsing::endsWith(u, ".jpg")) {
				if (Parsing::startsWith(u, "http")) {
					urlsTT.push_back(u);
				}
				else if (Parsing::startsWith(u, "/"))
				{
					std::string domain;
					size_t pos = 0;
					for (size_t i = 0; i < 3; i++)
					{
						pos = url.find("/", pos + 1);
					}
					domain = url.substr(0, pos);
					urlsTT.push_back(domain + u);
				}
				else
				{
					if (!Parsing::endsWith(url, "/"))
					{
						url += "/";
					}
					urlsTT.push_back(url + u);
				}
			}
			search = n;
		}
	} catch (HttpException e) {}
}
GuiScreenScreenshots::GuiScreenScreenshots():
	anim(this)
{
	CGE::instance->threadPool.runAsync([&] ()
	{
		imagesPage("http://alex2772.ru/screenshots/");
		if (!urlsTT.empty())
		{
			loadImage(urlsTT[index]);
		}
	});

	animations.push_back(new AnimationFade(this));
	addWidget(new GuiButton("<", [&](int, int, int)
	{
		if (!blur.load()) {
			if (urlsTT.empty())
				return;
			index--;
			index %= urlsTT.size();
			loadImage(urlsTT[index]);
		}
	}));
	addWidget(new GuiButton(">", [&](int, int, int)
	{
		if (!blur.load()) {
			if (urlsTT.empty())
				return;
			index++;
			index %= urlsTT.size();
			loadImage(urlsTT[index]);
		}
	}));
}

GuiScreenScreenshots::~GuiScreenScreenshots()
{
	CGE::instance->textureManager->unloadTexture(_R("gui/temp"));
}

void GuiScreenScreenshots::render(int flags, int mouseX, int mouseY)
{
	Gui::render(flags, mouseX, mouseY);
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(glm::vec4(1.f, 1.f, 1.f, .16f));
	glm::mat4 o = CGE::instance->renderer->transform;

	flare1 += CGE::instance->millis * 30;
	flare1 = fmod(flare1, CGE::instance->height * 1.5f);
	CGE::instance->renderer->transform = glm::rotate(CGE::instance->renderer->transform, glm::radians(45.f), glm::vec3(0, 0, 1));
	CGE::instance->renderer->drawRect(-CGE::instance->width / 2, flare1 - 10, CGE::instance->width * 3, 20);
	CGE::instance->renderer->setColor(glm::vec4(1.f, 1.f, 1.f, .07f));
	flare2 += CGE::instance->millis * 12;
	flare2 = fmod(flare2, CGE::instance->height * 1.5f);
	CGE::instance->renderer->drawRect(-CGE::instance->width / 2, flare2 - 100, CGE::instance->width * 3, 110);
	
	CGE::instance->renderer->transform = o;
	CGE::instance->renderer->blur(0, 0, CGE::instance->width, CGE::instance->height, 7);
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->setColor(glm::vec4(0, 0, 0.07f, 0.4f));
	CGE::instance->renderer->drawRect(0, 0, CGE::instance->width, CGE::instance->height);
;

	animTransform = glm::mat4(1.0);
	anim.process();
	CGE::instance->renderer->transform = CGE::instance->renderer->transform * animTransform;
	if (img)
	{
		const size_t Cwidth = GuiScreenScreenshots::width - 200;
		const size_t Cheight = GuiScreenScreenshots::height - 200;

		size_t width = img->width;
		size_t height = img->height;

		if (width > Cwidth)
		{
			height *= float(Cwidth) / float(width);
			width = Cwidth;
		}
		if (height > Cheight)
		{
			width *= float(Cheight) / float(height);
			height = Cheight;
		}

		CGE::instance->renderer->drawShadow((CGE::instance->width - width) / 2, (CGE::instance->height - height) / 2, width, height, glm::vec4(0, 0, 0, 1), 0, 13);
		calculateMask();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t.load());
		CGE::instance->renderer->setTexturingMode(1);
		CGE::instance->renderer->setColor(glm::vec4(1.f, 1.f, 1.f, 1.f));
		CGE::instance->renderer->drawRect((CGE::instance->width - width) / 2, (CGE::instance->height - height) / 2, width, height);
		CGE::instance->renderer->drawString((CGE::instance->width - width) / 2, (CGE::instance->height - height) / 2 - 34, imageName, Align::LEFT, TextStyle::SHADOW, glm::vec4(1.f), 26);
	}
	animTransform = glm::mat4(1.0);
	calculateMask();
	if (blur.load()) {
		CGE::instance->renderer->setTexturingMode(0);
		CGE::instance->renderer->setColor(glm::vec4(1.f, 1.f, 1.f, .3f));
		CGE::instance->renderer->blur(0, 0, width, height);
	}
}

void GuiScreenScreenshots::onLayoutChange(int x, int y, int _width, int _height)
{
	GuiScreen::onLayoutChange(x, y, _width, _height);
	widgets[0]->onLayoutChange(12, _height / 2 - 12, 25, 40);
	widgets[1]->onLayoutChange(_width - 38, _height / 2 - 12, 25, 40);
}

void GuiScreenScreenshots::close()
{
	animatedClose(animations[0]);
}

#endif