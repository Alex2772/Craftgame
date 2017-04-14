#include "GameEngine.h"

#ifndef SERVER
#include "glm/gtc/matrix_transform.hpp"
#include "Model.h"
#include "Models.h"
#include "Framebuffer.h"
#include "GuiScreenMainMenu.h"
#include "FModel.h"
#include "GuiConnectingPanel.h"
using namespace glm;
#endif
#include <vector>
#include "Dir.h"
#include <random>
#include <time.h>
#include "global.h"

#include "PacketDisconnected.h"
#include "PacketStartLogin.h"
#include "PacketPing.h"
#include "CommandList.h"
#include "CommandStop.h"
#include "OS.h"

CGE::GameEngine* CGE::instance = new CGE::GameEngine;

#ifdef SERVER
CGE::GameEngine::GameEngine():
	threadPool(4)
{
	packetRegistry = new PacketRegistry;
	commandRegistry = new CommandRegistry;
	logger = new Logger("server.log");
}
CGE::GameEngine::~GameEngine() {
	delete packetRegistry;
	delete settings;

	if (commandRegistry)
		delete commandRegistry;
}
#else
unsigned short fps = 0;
unsigned short fpss = 0;
time_t t = 0;

typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
static PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = nullptr;
#ifdef WINDOWS
void CGE_setVSync(bool sync)
{
	if (!wglSwapIntervalEXT) {
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
	}
	wglSwapIntervalEXT(sync);
}
#endif
CGE::GameEngine::GameEngine() :
	width(854),
	height(500),
	threadPool(4)
{
	logger = new Logger("client.log");
	vao = new VAO;
	renderer = new GameRenderer;
	imageLoader = new ImageLoader;
	textureManager = new TextureManager;
	fontRegistry = new FontRegistry;
	commandRegistry = new CommandRegistry;
	mainFB = nullptr;
	secFB = nullptr;
	thirdFB = nullptr;
	camera = new Camera;
	materialRegistry = new MaterialRegistry;
	modelRegistry = new ModelRegistry;
	packetRegistry = new PacketRegistry;
	if (FT_Init_FreeType(&ft)) {
		throw std::runtime_error("Failed to initializate FT Library");
	}
}
CGE::GameEngine::~GameEngine() {
	delete logger;
	delete standartGuiShader;
	delete blurShader;
	delete maskShader;
	delete vao;
	delete renderer;
	delete imageLoader;
	delete textureManager;
	delete fontRegistry;
	delete staticShader;
	delete materialRegistry;
	delete modelRegistry;
	delete connectingPanel;
	if (commandRegistry)
		delete commandRegistry;
	if (soundManager) {
		delete soundManager;
	}
	if (mainFB != nullptr) {
		delete mainFB;
		delete secFB;
		delete thirdFB;
	}
	if (camera != nullptr)
		delete camera;
	delete settings;
	while (guiScreens.size()) {
		delete guiScreens.back();
		guiScreens.pop_back();
	}
	while (light.size()) {
		delete light.back();
		light.pop_back();
	}
	if (gameProfile) {
		delete gameProfile;
	}
	if (server) {
		delete server;
		server = nullptr;
	}
	FT_Done_FreeType(ft);
}
#endif
void CGE::GameEngine::preInit() {
	settings = new Config::ConfigFile("settings.dat");

#ifndef SERVER
	settings->bindDefaultValue(_R("craftgame:graphics/normal"), true);
	settings->bindDefaultValue(_R("craftgame:debug"), false);
	settings->bindDefaultValue(_R("craftgame:graphics/maxFramerate"), int(0));
	settings->bindDefaultValue(_R("craftgame:graphics/animations"), true);
	connectingPanel = new GuiConnectingPanel;
	soundManager = new SoundManager;
	soundManager->initializeDevice(nullptr);
	soundManager->registerSound(new Sound(_R("craftgame:azaza"), "res/MFP.ogg"));
	soundManager->playSoundEffect(new SoundEffect(_R("craftgame:azaza"), glm::vec3(0, 0, 0), .5f, 1.f, true));
#endif

	size_t t = size_t(time(0));
	logger->info("Set srand seed to " + std::to_string(t));  
	srand(t);
	
	packetRegistry->registerPacket(new PacketStartLogin);
	packetRegistry->registerPacket(new PacketDisconnected);
	packetRegistry->registerPacket(new PacketPing);

	commandRegistry->registerCommand(new CommandList);
	commandRegistry->registerCommand(new CommandStop);

}
#ifndef SERVER
void CGE::GameEngine::postInit() {
	__CGE_CE("POSTINIT");
	Models::cube = new SimpleModel(std::vector<glm::vec3>({
		vec3(-1, -1, -1),
		vec3(-1, 1, -1),
		vec3(1, -1, -1),
		vec3(1, 1, -1),
		vec3(1, -1, -1),
		vec3(1, 1, -1),
		vec3(1, -1, 1),
		vec3(1, 1, 1),
		vec3(1, -1, 1),
		vec3(1, 1, 1),
		vec3(-1, -1, 1),
		vec3(-1, 1, 1),
		vec3(-1, -1, 1),
		vec3(-1, 1, 1),
		vec3(-1, -1, -1),
		vec3(-1, 1, -1),
		vec3(-1, -1, 1),
		vec3(-1, -1, -1),
		vec3(1, -1, 1),
		vec3(1, -1, -1),
		vec3(-1, 1, -1),
		vec3(-1, 1, 1),
		vec3(1, 1, -1),
		vec3(1, 1, 1)
	}),
		std::vector<glm::vec2>({
		vec2(0, 1),
		vec2(0, 0),
		vec2(1, 1),
		vec2(1, 0),
			vec2(0, 1),
			vec2(0, 0),
			vec2(1, 1),
			vec2(1, 0),
			vec2(0, 1),
			vec2(0, 0),
			vec2(1, 1),
			vec2(1, 0),
			vec2(0, 1),
			vec2(0, 0),
			vec2(1, 1),
			vec2(1, 0),
			vec2(0, 1),
			vec2(0, 0),
			vec2(1, 1),
			vec2(1, 0),
			vec2(0, 1),
			vec2(0, 0),
			vec2(1, 1),
			vec2(1, 0)
	}),
		std::vector<GLuint>({
		0, 1, 2, 2, 1, 3,
		4, 5, 6, 6, 5, 7,
		8, 9, 10, 10, 9, 11,
		12, 13, 14, 14, 13, 15,
		16, 17, 18, 18, 17, 19,
		20, 21, 22, 22, 21, 23
	}));

	Models::gui = new SimpleModel(std::vector<glm::vec3>({
		vec3(0, 0, 0),
		vec3(1, 0, 0),
		vec3(0, 1, 0),
		vec3(1, 1, 0)
	}),
		std::vector<glm::vec2>({
		vec2(0, 1),
		vec2(1, 1),
		vec2(0, 0),
		vec2(1, 0)
	}),
		std::vector<GLuint>({
		0, 1, 2, 2, 1, 3
	}));
	Models::viewport = new SimpleModel(std::vector<glm::vec3>({
		vec3(-1, -1, 0),
		vec3(1, -1, 0),
		vec3(-1, 1, 0),
		vec3(1, 1, 0)
	}),
		std::vector<glm::vec2>({
		vec2(0, 0),
		vec2(1, 0),
	    vec2(0, 1),
		vec2(1, 1)
	}),
		std::vector<GLuint>({
		0, 1, 2, 2, 1, 3
	}));

	/* Fonts */
	fontRegistry->registerFont(new Font("res/Roboto-Bold.ttf", _R("craftgame:default")));
	fontRegistry->registerFont(new Font("res/6563.ttf", _R("craftgame:futura")));
	fontRegistry->registerFont(new Font("res/futuralightc.otf", _R("craftgame:futuralight")));

	/* Materials */
	materialRegistry->registerMaterial(new Material(_R("craftgame:notexture"), CGE::instance->textureManager->loadTexture("res/notexture.png")));
	materialRegistry->registerMaterial(new Material(_R("craftgame:block/golden"), CGE::instance->textureManager->loadTexture("res/blocks/gold_block.png"), 0, glm::vec4(1, 1, 1, 1), 1.f, .5f));
	materialRegistry->registerMaterial(new Material(_R("craftgame:custom"), CGE::instance->textureManager->loadTexture("res/blocks/gold_block.png"), 0, glm::vec4(1, 1, 1, 1), 2.f, 0.1f));
	materialRegistry->registerMaterial(new Material(_R("craftgame:skin"), CGE::instance->textureManager->loadTexture("res/entity/default.png"), 0, glm::vec4(0.7, 0.7, 0.7, 1)));
	materialRegistry->registerMaterial(new Material(_R("craftgame:surface"), CGE::instance->textureManager->loadTexture("res/blocks/gold_block.png"), 0, glm::vec4(0.3, 0.3, 0.3, 1)));

	/* Models */
	modelRegistry->registerModel("res/models/mainmenu.cgem", _R("craftgame:menu/main"), false);
	modelRegistry->registerModel("res/models/cube.cgem", _R("craftgame:cube"));

	standartGuiShader = new Shader("res/glsl/guiV.glsl", "res/glsl/guiF.glsl");
	standartGuiShader->bindAttribute("position");
	standartGuiShader->bindAttribute("texCoords");
	standartGuiShader->link();
	standartGuiShader->start();
	standartGuiShader->loadInt("useProjection", 0);
	standartGuiShader->loadInt("tex", 0);
	standartGuiShader->loadInt("fb", 1);
	standartGuiShader->loadFloat("blur", 0);
	standartGuiShader->loadInt("texturing", 1);
	standartGuiShader->loadInt("mask", 4);
	standartGuiShader->stop();

	maskShader = new Shader("res/glsl/maskV.glsl", "res/glsl/maskF.glsl");
	maskShader->bindAttribute("position");
	maskShader->bindAttribute("texCoords");
	maskShader->link();

	guiShader = standartGuiShader;
	blurShader = new BlurShader("res/glsl/blurV.glsl", "res/glsl/blurF.glsl");
	blurShader->bindAttribute("position");
	blurShader->bindAttribute("texCoords");
	blurShader->link();
	blurShader->start();
	blurShader->loadInt(guiShader->getUniform("useProjection"), 0);
	blurShader->loadVector("c", glm::vec4(1, 1, 1, 1));
	blurShader->loadInt(guiShader->getUniform("fb"), 0);
	blurShader->loadFloat(guiShader->getUniform("orientation"), 0);
	blurShader->setBlurRadius(13);
	blurShader->stop();

	staticShader = new Shader("res/glsl/staticV.glsl", "res/glsl/staticF.glsl");
	staticShader->bindAttribute("position");
	staticShader->bindAttribute("texCoords");
	staticShader->bindAttribute("normal");
	staticShader->bindAttribute("tangent");
	staticShader->link();
	staticShader->start();
	staticShader->loadInt("material.tex", 0);
	staticShader->loadInt("material.normal", 1);
	staticShader->loadInt("texturing", 1);
	staticShader->loadVector("c", glm::vec4(1, 1, 1, 1));
	staticShader->stop();
	sizeSpecific();
	guiShader->start();
	displayGuiScreen(new GuiScreenMainMenu());

	CGE_setVSync(settings->getProperty<int>(_R("craftgame:graphics/maxFramerate")) == 0);
}
void CGE::GameEngine::onResize(int _width, int _height) {
	__CGE_CE("RESIZE");
	width = _width;
	height = _height;
	sizeSpecific();
	for (std::deque<GuiScreen*>::iterator i = guiScreens.begin(); i != guiScreens.end(); i++) {
		(*i)->onLayoutChange(0, 0, _width, _height);
	}
	connectingPanel->onLayoutChange(width - 750, height - 300, 750, 300);
}
void CGE::GameEngine::sizeSpecific() {
	__CGE_CE("SIZE SPECIFIC");
	if (width && height) {
		renderer->changeViewport(0, 0, width, height);

		if (mainFB) {
			delete mainFB;
			delete secFB;
			delete thirdFB;
			delete maskFB;
		}
		if (glewInitiated) {
			mainFB = new Framebuffer(width, height);
			mainFB->bind(width, height);
			renderer->changeViewport(0, 0, width, height);
			mainFB->unbind();
			secFB = new SecFramebuffer(width, height);
			secFB->bind(width, height);
			renderer->changeViewport(0, 0, width, height);
			secFB->unbind();
			thirdFB = new SecFramebuffer(width, height);
			thirdFB->bind(width, height);
			renderer->changeViewport(0, 0, width, height);
			thirdFB->unbind();
			maskFB = new MaskFramebuffer(width, height);
			maskFB->bind(width, height);
			renderer->changeViewport(0, 0, width, height);
			maskFB->unbind();

			projection = glm::perspective(glm::radians(75.f), float(width) / float(height), 0.01f, 1000.f);
			projection2 = glm::perspective(glm::radians(75.f), 1.f, 0.01f, 1000.f);
			staticShader->start();
			staticShader->loadMatrix("projection", projection);
			staticShader->stop();
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, CGE::instance->maskFB->texture);
			glActiveTexture(GL_TEXTURE0);
		}
	}
	__CGE_CE("AFTER SIZE SPECIFIC");
}

void CGE::GameEngine::render(int mouseX, int mouseY) {
	__CGE_CE("START RENDER");
	auto renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	static bool overlayCheck = true;
	static bool drag = true;
	size_t transform = guiShader->getUniform("transform");
	static size_t background = textureManager->loadTexture("res/blocks/dirt.png");
	static size_t cobblestone = textureManager->loadTexture("res/blocks/cobblestone.png");
	static size_t color = guiShader->getUniform("c");
	size_t uv = CGE::instance->guiShader->getUniform("uv");
	size_t uv2 = CGE::instance->guiShader->getUniform("uv2");
	static size_t scales = CGE::instance->blurShader->getUniform("scales");
	static size_t view = CGE::instance->staticShader->getUniform("view");
	static size_t cursor = CGE::instance->textureManager->loadTexture("res/cursor.png", true);

	if (t < time(0)) {
		t = time(0);
		fpss = fps;
		fps = 0;
	}


	__CGE_CE("UI THREAD");
	while (uiThread.size()) {
		auto t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
		uiThread.front()();
		uiThread.pop();
		t = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - t;
		//INFO(std::string("Last post request took ") + std::to_string(t.count()) + " ms");
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	__CGE_CE("LOADING DATA INTO SHADERS");
	staticShader->start();

	staticShader->loadMatrix(view, camera->createViewMatrix());

	for (int i = 0; i < light.size(); i++) {
		staticShader->loadVector("light[" + std::to_string(i) + "].pos", light[i]->pos);
		staticShader->loadVector("light[" + std::to_string(i) + "].color", light[i]->color);
	}

	blurShader->start();
	blurShader->loadVector(scales, glm::vec2(1.f / float(width), 1.f / float(height)));
	
	guiShader->start();
	mainFB->bind(width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GameEngine::mouseX = mouseX;
	GameEngine::mouseY = mouseY;


	__CGE_CE("RENDERING GUI SCREEN");
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	guiShader->loadInt("masking", 1);
	for (std::deque<GuiScreen*>::iterator i = guiScreens.begin(); i != guiScreens.end(); i++) {
		GuiScreen* s = *i;
		s->render(guiScreens.end() - 1 == i ? RenderFlags::STANDART : (RenderFlags::STANDART | RenderFlags::OVERLAYED), mouseX, mouseY);
		s->renderWidgets(guiScreens.end() - 1 == i ? RenderFlags::STANDART : (RenderFlags::STANDART | RenderFlags::OVERLAYED), mouseX, mouseY);
	}
	__CGE_CE("RENDERING CONNECTING PANEL");
	connectingPanel->render(RenderFlags::STANDART, -connectingPanel->x + mouseX, -connectingPanel->y + mouseY);
	guiShader->loadInt("masking", 0);
	renderer->transform = glm::mat4(1.0);
	__CGE_CE("RENDERING DEBUG SCREEN");
	bool* debug = settings->getPropertyPointer<bool>(_R("craftgame:debug"));
	if (fpss <= 10 || *debug) {
		glm::vec4 c = glm::mix(glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), glm::min(float(fpss) / 60.f, 1.f));
		renderer->drawString(2, 2, std::to_string(fpss) + std::string(" FPS"), Align::LEFT, TextStyle::SHADOW, c);
		if (fpss <= 10) {
			c.a = (cos(float(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 1000) / 1000.f * 20 / 3.14) + 1) / 2;
			renderer->drawString(80, 2, "!", Align::LEFT, TextStyle::SHADOW, c);
		}
	}
	__CGE_CE("DISPAYING INTO SCREEN");
	guiShader->loadVector("c", glm::vec4(1, 1, 1, 1));
	mainFB->bind(width, height);
	glDisable(GL_BLEND);
	mainFB->unbind();
	guiShader->loadMatrix(transform, glm::mat4(1));
	guiShader->loadVector(uv, glm::vec2(0, 0));
	guiShader->loadVector(uv2, glm::vec2(1, 1));
	glBindTexture(GL_TEXTURE_2D, mainFB->texture);
	renderer->renderModel(Models::viewport);
	glBindTexture(GL_TEXTURE_2D, cursor);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	CGE::instance->renderer->drawRect(mouseX, mouseY, 32, 32);
	glDisable(GL_BLEND);
	guiShader->stop();

	__CGE_CE("COUNTING FPS AND MILLIS");
	fps++;
	renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - renderTime;
	int maxFramerate = settings->getProperty<int>(_R("craftgame:graphics/maxFramerate"));
	if (maxFramerate > 0 && maxFramerate < 13) {
		int timeForFrame = 1000 / (maxFramerate * 10);
		int rt = renderTime.count();
		if (rt < timeForFrame) {
			std::this_thread::sleep_for(std::chrono::milliseconds(timeForFrame - rt));
		}
	}

}
void CGE::GameEngine::onClick(int btn) {
	uiThread.push(std::function<void()>([&, btn](){
		if (guiScreens.size())
			guiScreens.back()->onMouseClick(mouseX, mouseY, btn);
	}));
}
void CGE::GameEngine::displayGuiScreen(GuiScreen* g) {
	uiThread.push(std::function<void()>([&, g]() {
		guiScreens.push_back(g);
		g->onLayoutChange(0, 0, width, height);
	}));
}
void CGE::GameEngine::removeGuiScreen(GuiScreen* s) {
	uiThread.push(std::function<void()>([&, s]() {
		for (std::deque<GuiScreen*>::iterator i = guiScreens.begin(); i != guiScreens.end(); i++) {
			if (*i == s) {
				delete s;
				guiScreens.erase(i);
				break;
			}
		}
	}));
}
void CGE::GameEngine::onKeyDown(Keyboard::Key key, byte data) {
	uiThread.push(std::function<void()>([&, key, data]() {
		if (guiScreens.size())
			guiScreens.back()->onKeyDown(key, data);
		if (data & 1) {
			switch (key) {
			case Keyboard::Key::F2:
				Dir d("../screenshots");
				if (!d.isDir())
					d.mkdir();

				Image image;

				size_t sizes = 3 * width * height;
				unsigned char* buffer = new unsigned char[sizes];
				image.data = new unsigned char[sizes];
				glBindTexture(GL_TEXTURE_2D, mainFB->texture);
				glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);
				glPixelStorei(GL_PACK_ALIGNMENT, 1);
				glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						size_t index = y * width * 3 + x * 3;
						size_t index1 = (height - y - 1) * width * 3 + x * 3;
						image.data[index] = buffer[index1];
						image.data[index + 1] = buffer[index1 + 1];
						image.data[index + 2] = buffer[index1 + 2];
					}
				}
				image.width = width;
				image.height = height;
				std::stringstream ss;
				ss << "screenshot-";

				std::time_t t = std::time(NULL);
				std::tm tm;
				localtime_s(&tm, &t);
				char timebuf[64];
				std::strftime(timebuf, sizeof(timebuf), "%F_%H.%M.%S", &tm);
				ss << timebuf << ".png";
				imageLoader->savePNG(&image, "../screenshots/" + ss.str());
				delete[] buffer;
				logger->info("Screenshort saved as " + ss.str());
				break;
			}
		}
	}));
}
void CGE::GameEngine::onWheel(short d) {
	uiThread.push(std::function<void()>([&, d]() {
		if (guiScreens.size())
			guiScreens.back()->onWheel(mouseX, mouseY, d);
	}));
}
void CGE::GameEngine::onText(char c) {
	uiThread.push(std::function<void()>([&, c]() {
		if (guiScreens.size())
			guiScreens.back()->onText(c);
	}));
}
#include "GuiScreenMessageDialog.h"
unsigned char CGE::GameEngine::displayDialog(std::string title, std::string text) {
	unsigned char result;
	result = 253;
	GuiScreen* dialog = nullptr;
	uiThread.push(std::function<void()>([&, title, text]() {
		displayGuiScreen(dialog = new GuiScreenMessageDialog(title, text, &result));
		result = 254;
	}));
	while (result == 253 || result == 254) {
		OS::sleep(1);
	}
	return result;
}

void CGE::GameEngine::disconnect(Socket* s, std::string reason, std::string title) {
	if (s) {
		s->close();
		delete s;
	}
	CGE::instance->uiThread.push([&, reason, title]() {
		connectingPanel->collapse();
		if (CGE::instance->server) {
			CGE::instance->server->runOnServerThread([&]() {
				delete CGE::instance->server;
				CGE::instance->server = nullptr;
			});
		}
		if (guiScreens.size() < 2) {
			while (guiScreens.size()) {
				delete guiScreens.back();
				guiScreens.pop_back();
			}
		}
		displayGuiScreen(new GuiScreenMainMenu);
		CGE::instance->threadPool.runAsync([&, reason, title]() {
			displayDialog(title, reason);
		});
	});
}
#include "RemoteServer.h"
#include "PacketStartLogin.h"
#include "AnimationDialog.h"
#include "PacketPing.h"
void CGE::GameEngine::connectToServer(std::string url, unsigned short port) {
	
	std::thread* t = new std::thread([&, url, port]() {
		try {
			CGE::instance->uiThread.push([&, url, port]() {
				connectingPanel->displayTop = true;
				connectingPanel->show();
				connectingPanel->resetTimer();
				connectingPanel->first = url + ":" + std::to_string(port);
				connectingPanel->second = "FINDING SERVER";
			});
			OS::sleep(1000);
			CGE::instance->socket = new Socket(url, port);
			CGE::instance->socket->write<unsigned char>(0x3c);
			unsigned char a;
			CGE::instance->socket->readBytes((char*)&a, 1, 0);
			if (a == 0xac) {
				CGE::instance->uiThread.push([&]() {
					connectingPanel->collapse();
				});
				OS::sleep(500);
				CGE::instance->uiThread.push([&]() {
					connectingPanel->second = "CONNECTING";
					connectingPanel->displayTop = false;
					connectingPanel->show();
				});
				Socket* s = CGE::instance->socket;
				s->write<size_t>(VERSION_CODE);
				char d;
				s->readBytes((char*)&d, 1, 0);
				if (d < 0) {
					std::string st;
					switch (d ^ 0xF0) {
					case 0:
						st = "VERSION_INCORRECT";
						break;
					case 1:
						st = "AUTH_FAILED";
						break;
					default:
						st = std::string("code ") + std::to_string(int(d) ^ 0xf0);
					}
					disconnect(s, std::string("Server declined connection (status ") + st + ")");
				}
				size_t id = s->read<size_t>();
				RemoteServer* r = new RemoteServer(s);
				CGE::instance->server = r;
				r->sendPacket(new PacketStartLogin(gameProfile->getUsername(), id));
				CGE::instance->uiThread.push([&]() {
					connectingPanel->collapse();
				});
				OS::sleep(500);
				CGE::instance->uiThread.push([&]() {
					if (guiScreens.size()) {
						Animation* a = new AnimationDialog(guiScreens.front());
						a->state = State::inverse;
						a->onAnimationEnd = std::function<void(State)>([&](State s) {
							if (s == State::inverse) {
								CGE::instance->uiThread.push(std::function<void()>([&, s]() {
									while (guiScreens.size()) {
										delete guiScreens.back();
										guiScreens.pop_back();
									}
								}));
							}
						});
						guiScreens.front()->animations.push_back(a);
					}
				});
				CGE::instance->threadPool.runAsync([r]() {
					while (CGE::instance->server) {
						r->runOnServerThread([r]() {
							r->sendPacket(new PacketPing());
						});
						OS::sleep(5000);
					}
				});
				while (CGE::instance->server) {
					auto chrono = std::chrono::system_clock::now();
					server->tick();
					size_t millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - chrono).count();
					if (millis < 50)
						OS::sleep(50 - millis);
				}
			}
			else {
				INFO(std::string("Incorrect server with byte ") + std::to_string(a));
				CGE::instance->uiThread.push([&]() {

				});
			}
		}
		catch ( CraftgameException e) {
			connectingPanel->collapse();
			CGE::instance->displayDialog("Connection error", e.what());
		}
	});
	
}

void CGE::GameEngine::onSecondContext() {
	if (!secondThread.empty()) {
		secondThread.front()();
		secondThread.pop();
	}

}
#endif
//=====================================================================================//
/*                          BOTH CLIENT AND SERVER METHODS                             */
//=====================================================================================//
size_t CGE::GameEngine::assignGlobalUniqueEntityId() {
	return entityId++;
}
std::string __prev = "";
void CGE::checkOpenGLError(std::string s)
{
	GLenum err;
	if (__prev.empty())
		__prev = s;
	if ((err = glGetError()) != GL_NO_ERROR) {
		std::stringstream ss;
		ss << "OpenGL error has occurred ";
		ss << "[" << __prev << "] => [" << s << "]";
		char* d = (char*)gluGetString(err);
		if (d)
			ss << ": " << d;
		CGE::instance->logger->err(ss.str()));
	}
	__prev = s;
}
