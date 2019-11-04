#include "GameEngine.h"
#include <boost/locale.hpp>
#include <curl/curl.h>
#include <boost/make_shared.hpp>
#include "Random.h"
#include "Blocks.h"
#include "Items.h"
#include "SolidBlock.h"
#include "CommandAlert.h"
#include "PlayerController.h"
#include "DirStreamProvider.h"
#include "BinaryInputStream.h"
#include "BufferedStreamProvider.h"
#include <marshal.h>
#include "GuiInventory.h"
#include "ModelItemRenderer.h"
#include "EntityItem.h"
#include "EntityYoYo.h"
#include "EntityOtherPlayerMP.h"
#include "POpenInventory.h"
#include "P02JoinWorld.h"
#include "ServerConnect.h"
#include "GuiScreenRegister.h"
#include "CommandTickrate.h"
#include "PForce.h"
#include "PlayerListEntry.h"
#include "P00Disconnected.h"
#include <boost/algorithm/string/predicate.hpp>
#include "EntityMutant.h"
#ifndef SERVER
#include "glm/gtc/matrix_transform.hpp"
#include "Model.h"
#include "Models.h"
#include "Framebuffer.h"
#include "GuiScreenMainMenu.h"
//#include "GuiWindowDebug.h"
#include "GuiScreenLoad.h"
#include "GuiScreenMessageDialog.h"
#include "GuiScreenChat.h"
#include "GuiScreenPause.h"
using namespace glm;
#endif
#include <vector>
#include "Dir.h"
#include <random>
#include <time.h>
#include "global.h"
#include "CommandKick.h"
#include "CommandList.h"
#include "CommandStop.h"
#include "CommandHelp.h"
#include "CommandAlert.h"
#include "CommandTime.h"
#include "OS.h"
#include "Joystick.h"
#include "TextureAtlas.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "CommandTPPos.h"
#include "RemoteServer.h"

// PYTHON MODULES =================================================
#include "pycge.h"
#include "pylogger.h"
#include "PEntityLook.h"
#include "PADTUpdate.h"
#include "PAutocomplete.h"
#include "PSound.h"
#include "CommandSetblock.h"
//#include "pyhook.h"

// ================================================================
CGE::GameEngine* CGE::instance = new CGE::GameEngine;

#ifdef SERVER
CGE::GameEngine::GameEngine():
	threadPool(8)
{
	packetRegistry = new PacketRegistry;
	commandRegistry = new CommandRegistry;
	currentLocale = new Locale("res/lang/en_US.lang");
	logger = new Logger("server.log");
	modManager = new ModManager;
	goRegistry = new GORegistry;
	entityRegistry = new EntityRegistry;
	materialRegistry = new MaterialRegistry;
	modelRegistry = new ModelRegistry;
	chat = new Chat;
	if (curl_global_init(CURL_GLOBAL_ALL) != 0)
	{
		throw std::runtime_error("Failed to initializate cURL library");
	}

}
CGE::GameEngine::~GameEngine() {
	delete packetRegistry;
	delete settings;
	delete modManager;
	delete goRegistry;
	delete entityRegistry;
	delete materialRegistry;
	delete modelRegistry;
	delete chat;
	server.destruct();
	if (commandRegistry)
		delete commandRegistry;
	curl_global_cleanup();
}
#else
unsigned short fps = 0;
unsigned short fpss = 0;
time_t t = 0;
extern size_t _gDrawCalls;

#ifdef WINDOWS
typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALPROC)(int);
static PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = nullptr;
void CGE_setVSync(bool sync)
{
	if (!wglSwapIntervalEXT) {
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
	}
	wglSwapIntervalEXT(sync);
}
#else
void CGE_setVSync(bool v) {

}
#endif
CGE::GameEngine::GameEngine() :
	width(854),
	height(500),
	threadPool(4)
{
	chat = new Chat;
	logger = new Logger("client.log");
	vao = new VAOHelper;
	renderer = new GameRenderer;
	imageLoader = new ImageLoader;
	textureManager = new TextureManager;
	fontRegistry = new CFontRegistry;
	commandRegistry = new CommandRegistry;
	mainFB = nullptr;
	secFB = nullptr;
	thirdFB = nullptr;
	shadowFB = nullptr;
	camera = new Camera;
	materialRegistry = new MaterialRegistry;
	modelRegistry = new ModelRegistry;
	goRegistry = new GORegistry;
	packetRegistry = new PacketRegistry;
	currentLocale = new Locale(OS::locale());
	kbRegistry = new KeyBindingRegistry;
	modManager = new ModManager;
	entityRegistry = new EntityRegistry;
	light.setObject(new std::vector<LightSource*>);
	if (FT_Init_FreeType(&ft)) {
		throw std::runtime_error("Failed to initializate FT Library");
	}
	if (curl_global_init(CURL_GLOBAL_ALL) != 0)
	{
		throw std::runtime_error("Failed to initializate cURL library");
	}
}
extern bool __3rd_person;
CGE::GameEngine::~GameEngine() {
	ts<IServer>::rw l(server);
	if (server.isValid())
	{
		server.destruct();
	}
/*
	while (!guiScreens.empty()) {
		delete guiScreens.back();
		guiScreens.pop_back();
	}
	*/
	while (!guiWindows.empty()) {
		delete guiWindows.back();
		guiWindows.pop_back();
	}
	delete logger;
	delete standartGuiShader;
	delete starShader;
	delete particleShader;
	delete particleShader2;
	delete blurShader;
	delete maskShader;
	for (size_t i = 0; i < gbufferShader.size(); i++) {
		delete gbufferShader[i];
	}
	delete vao;
	delete modManager;
	delete renderer;
	delete imageLoader;
	delete textureManager;
	delete fontRegistry;
	delete defaultStaticShader;
	delete materialRegistry;
	delete modelRegistry;
	delete goRegistry;
	delete entityRegistry;
	delete kbRegistry;
	if (thePlayer)
	{
		delete thePlayer;
		thePlayer = nullptr;
	}
	if (commandRegistry)
		delete commandRegistry;
	if (soundManager) {
		SoundManager* n = soundManager;
		soundManager = nullptr;
		delete n;
	}
	if (mainFB != nullptr) {
		delete mainFB;
		mainFB = nullptr;
		delete secFB;
		delete thirdFB;
		delete gBuffer;
		delete shadowFB;
	}
	if (camera != nullptr)
		delete camera;
	delete settings;
	if (gameProfile) {
		delete gameProfile;
	}
	FT_Done_FreeType(ft);
	curl_global_cleanup();
}
#endif
void CGE::GameEngine::preInit() {
	logger->setThreadName("main thread");
	settings = new Config::ConfigFile("settings.dat");

#ifndef SERVER

	settings->bindDefaultValue(_R("craftgame:graphics/normal"), true);
	settings->bindDefaultValue(_R("craftgame:debug"), false);
	settings->bindDefaultValue(_R("craftgame:net_debug"), false);
	settings->bindDefaultValue(_R("craftgame:dev"), false);
	settings->bindDefaultValue(_R("craftgame:graphics/maxFramerate"), int(0));
	settings->bindDefaultValue(_R("craftgame:graphics/animations"), true);
	settings->bindDefaultValue(_R("craftgame:graphics/sky"), int(1));
	settings->bindDefaultValue(_R("craftgame:graphics/lighting"), int(2));
	settings->bindDefaultValue(_R("craftgame:graphics/ao"), int(2));
	settings->bindDefaultValue(_R("craftgame:graphics/reflections"), int(2));
	settings->bindDefaultValue(_R("craftgame:gui/blur"), true);
	settings->bindDefaultValue(_R("craftgame:gui/firstplay"), true);
	settings->bindDefaultValue<std::string>("multiplayer/lastserver", "");
	settings->bindDefaultValue<std::string>("gameprofile/username", "");
	{
		KeyBindingGroup* gr = new KeyBindingGroup("multiplayer");
		gr->data.push_back(new KeyBinding("chat", Keyboard::T, [&]()
		{
			if (guiScreens.size() <= 1)
			{
				displayGuiScreen(new GuiScreenChat());
			}
		}));
		kbRegistry->registerKeyBinding(gr);
	}
	{
		KeyBindingGroup* gr = new KeyBindingGroup("gameplay");
		gr->data.push_back(new KeyBinding("3rdperson", Keyboard::F5, [&]()
		{
			__3rd_person = !__3rd_person;
		}));
		gr->data.push_back(new KeyBinding("inventory", Keyboard::E, [&]()
		{
			if (guiScreens.empty()) {
				thePlayer->openInterface(new CSInterface(&thePlayer->interface, {}));
				ts<IServer>::r(server)->sendPacket(new POpenInventory);
			}
		}));
		kbRegistry->registerKeyBinding(gr);
	}


	kbRegistry->load(settings);
#endif

	size_t t = size_t(time(0));
	logger->info("Set srand seed to " + std::to_string(t));  
	srand(t);

	entityRegistry->registerEntity<EntityOtherPlayerMP>();
	entityRegistry->registerEntity<EntityItem>();
	entityRegistry->registerEntity<EntityYoYo>();
	entityRegistry->registerEntity<EntityMutant>();

	Packets::reg();

	commandRegistry->registerCommand(new CommandList);
	commandRegistry->registerCommand(new CommandStop);
	commandRegistry->registerCommand(new CommandKick);
	commandRegistry->registerCommand(new CommandHelp);
	commandRegistry->registerCommand(new CommandAlert);
	commandRegistry->registerCommand(new CommandTPPos);
	commandRegistry->registerCommand(new CommandTime);
	commandRegistry->registerCommand(new CommandTickrate);
	commandRegistry->registerCommand(new CommandSetblock);

}
void CGE::GameEngine::createThread(std::function<void()> f)
{
	threads.push_back(new std::thread(f));
}


#ifndef SERVER
void CGE::GameEngine::skipInputEventRejection()
{
	inputEvent = 0;
}

void CGE::GameEngine::setDebugString(std::string key, std::string value)
{
	for (std::vector<DebugInfo>::iterator i = debugInfo.begin(); i != debugInfo.end(); i++)
	{
		if (i->key == key)
		{
			if (i->value != value)
			{
				i->highlightAnimation = 1.f;
			}
			i->value = value;
			return;
		}
	}
	DebugInfo d;
	d.key = key;
	d.value = value;
	debugInfo.push_back(d);
}


Shader* __createGBufferShader(std::string fr)
{
	CGE::checkOpenGLError(std::string("GBuffer ") + fr);
	Shader * gbufferShader = new Shader("res/glsl/gbufferV.glsl", fr);
	gbufferShader->onCompile = [gbufferShader]()
	{
		{
			int s = 4;
			switch (CGE::instance->settings->getProperty<int>("graphics/ao"))
			{
			case 0:
				s = 0;
				break;
			case 1:
				s = 8;
				break;
			case 2:
				s = 16;
				break;
			case 3:
				s = 32;
				break;
			}
			gbufferShader->defines["SSAO_SAMPLES"] = s;
		}
		{
			int s = 0;
			switch (CGE::instance->settings->getProperty<int>("graphics/reflections"))
			{
			case 0:
				s = 0;
				break;
			case 1:
				s = 2;
				break;
			case 2:
				s = 8;
				break;
			default:
				s = 0;
			}
			gbufferShader->defines["REFLECTIONS"] = s;
		}
		{
			gbufferShader->defines["SKY"] = CGE::instance->settings->getProperty<int>("graphics/sky");
		}
	};
	gbufferShader->onUpload = [gbufferShader, fr]()
	{
		if (CGE::instance->gbufferShader.empty()) {
			gbufferShader->loadInt("sun", 0);
			gbufferShader->loadInt("shadow1", 5); // Только для первого шейдера
		}
		gbufferShader->loadInt("depth", 0);
		gbufferShader->loadInt("diffuse", 1);
		gbufferShader->loadInt("normal", 2);
		gbufferShader->loadInt("f0", 3);
		gbufferShader->loadInt("noise", 6);
		if (gbufferShader->getUniform("kernel[0]")) {
			gbufferShader->loadVector("screen", glm::vec2(CGE::instance->width, CGE::instance->height));
			const size_t COUNT = 32;
			for (size_t i = 0; i < COUNT; i++)
			{
				glm::vec3 kek = Random::nextVec3();
				kek.x = kek.x * 2 - 1;
				kek.y = kek.y * 2 - 1;
				kek.z = kek.z * 2 - 1;
				kek = glm::normalize(kek);
				kek *= float(i + 1.f) / float(COUNT);
				gbufferShader->loadVector(std::string("kernel[") + std::to_string(i) + "]", kek);
			}
		}
	};
	gbufferShader->defines["GRAPHICS"] = CGE::instance->settings->getProperty<int>("graphics/lighting");
	gbufferShader->compile();
	gbufferShader->bindAttribute("position");
	gbufferShader->bindAttribute("texCoords");
	gbufferShader->link();
	gbufferShader->start();
	CGE::instance->gbufferShader.push_back(gbufferShader);
	return gbufferShader;
}

void _jump()
{
	static std::chrono::milliseconds last(0);
	std::chrono::milliseconds now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock().now().time_since_epoch());
	if (now.count() - last.count() < 250)
	{
		CGE::instance->thePlayer->setFlying(!CGE::instance->thePlayer->isFlying());
		last = std::chrono::milliseconds(0);
	}
	else
	{
		last = now;
	}
}

void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR) {
		std::stringstream ss;
		ss << "OpenGL error has occurred :: " << message;
		CGE::instance->logger->err(ss.str());
		assert(0);
	}
}
void compileStaticShader(Shader* s)
{
	s->compile();
	s->bindAttribute("position");
	s->bindAttribute("texCoords");
	s->bindAttribute("normal");
	s->bindAttribute("tangent");
	s->link();
	s->start();
	s->loadInt("block", 0);
	s->loadInt("lightingC", 0);
	s->loadInt("shadows", 0);
	s->loadInt("material.tex", 0);
	s->loadInt("material.normal", 1);
	s->loadInt("material2.tex", 3);
	s->loadInt("material2.normal", 4);
	s->loadVector("c", glm::vec4(1, 1, 1, 1));
	s->loadInt("layer", 0);
	s->loadInt("texturing", 1);
	s->stop();
}
void compileSkeletonShader(Shader* s)
{
	s->compile();
	s->bindAttribute("position");
	s->bindAttribute("texCoords");
	s->bindAttribute("normal");
	s->bindAttribute("tangent");
	s->bindAttribute("boneIDs1");
	s->bindAttribute("boneWeights1");
	s->bindAttribute("boneIDs2");
	s->bindAttribute("boneWeights2");
	s->link();
	s->use();
	s->loadInt("lightingC", 0);
	s->loadInt("material.tex", 0);
	s->loadInt("material.normal", 1);

	s->loadInt("material2.tex", 3);
	s->loadInt("material2.normal", 4);
	s->loadVector("c", glm::vec4(1, 1, 1, 1));
	s->loadInt("layer", 0);
	s->stop();
}
void CGE::GameEngine::init() {
#ifdef _DEBUG
	glDebugMessageCallback(glDebugCallback, 0);
	glGetError();
#endif
	checkOpenGLError("Init start");
	Models::selection = new SimpleModel(std::vector<glm::vec3>({
		vec3(-1, -1, -1), // 0
		vec3(1, -1, -1), // 1
		vec3(-1, -1, 1), // 2
		vec3(1, -1, 1), // 3

		vec3(-1, 1, -1), // 4
		vec3(1, 1, -1), // 5
		vec3(-1, 1, 1), // 6
		vec3(1, 1, 1), // 7
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
	}),
		std::vector<GLuint>({
			0, 4,
			1, 5,
			2, 6,
			3, 7,

			0, 1,
			1, 3,
			3, 2,
			2, 0,

			4, 5,
			5, 7,
			7, 6,
			6, 4
	}));
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

	/* CFonts */
	fontRegistry->registerCFont(new CFont("res/Roboto-Bold.ttf", _R("craftgame:default")));

	checkOpenGLError("Materials");
	/* Materials */
	{
		Image* img = new Image;
		img->alpha = false;
		img->width = img->height = 2;
		const uint8_t r[] = {
			0, 0, 0,
			255, 0, 255,
			255, 0, 255,
			0, 255, 0,
		};
		uint8_t* data = new uint8_t[sizeof(r)];
		memcpy(data, r, sizeof(r));
		img->data = data;
		materialRegistry->registerMaterial((new Material(_R("craftgame:notexture")))->setTexture(textureManager->loadTexture(img, _R("craftgame:notexture"), false)));
	}
	{
		Image* img = new Image;
		img->type = GL_FLOAT;
		img->alpha = false;


		std::vector<glm::vec3> colors;
		const size_t COUNT = 16;
		for (size_t i = 0; i < COUNT; i++)
		{
			glm::vec3 kek = Random::nextVec3();
			kek.x = kek.x * 2 - 1;
			kek.y = kek.y * 2 - 1;
			kek.z = kek.z * 2 - 1;
			colors.push_back(kek);

		}
		size_t
			side = 2;
		while (side * side < colors.size())
			side *= 2;
		img->width = img->height = side;
		size_t s = side * side * 3;
		float* data = new float[s];

		size_t index = 0;
		for (glm::vec3& kek : colors) {
			data[index++] = kek.x;
			data[index++] = kek.y;
			data[index++] = kek.z;
		}
		img->data = (unsigned char*)data;
		INFO("Generated " + std::to_string(side) + "x" + std::to_string(side) + " SSAO noise texture");
		materialRegistry->registerMaterial((new Material(_R("craftgame:ssao4x4")))->setTexture(textureManager->loadTexture(img, _R("craftgame:ssao4x4"), false)));
	}
	time_t t = std::time(0);
	tm* timeinfo = localtime(&t);
	materialRegistry->registerMaterial((new Material(_R("craftgame:custom"))));
	{
		Material* m = (new Material(_R("craftgame:skin")))->setTexture(CGE::instance->textureManager->loadTexture(_R("craftgame:res/entity/default.png")))->setColor(glm::vec4(0.7, 0.7, 0.7, 1));
		if (timeinfo->tm_mon == 3 && timeinfo->tm_mday == 1)
		{
			m->set2Layer((new SecondMaterial("craftgame:skin_/1a"))->setTexture(CGE::instance->textureManager->loadTexture("res/materials/1a_overlay.png")));
		}
		materialRegistry->registerMaterial(m);
	}
	materialRegistry->registerMaterial((new Material(_R("craftgame:surface")))->setColor(glm::vec4(0.5, 0.5, 0.5, 1)));
	materialRegistry->registerMaterial((new Material(_R("craftgame:entity/dimon/armor")))->setColor(glm::vec4(0.35, 0.35, 0.35, 1))->setMetal(true)->setRoughness(0.87f)
		->setNormalMap(CGE::instance->textureManager->loadTexture("res/blocks/noise-normal.png")));
	materialRegistry->registerMaterial((new Material(_R("craftgame:entity/dimon/body")))->setTexture(CGE::instance->textureManager->loadTexture("res/blocks/stone.png")));

	checkOpenGLError("Models");
	/* Models */
	modelRegistry->registerModel("res/models/cube.cgem", _R("craftgame:cube"));
	modelRegistry->registerModel("res/models/sphere.cgem", _R("craftgame:sphere"));

	checkOpenGLError("Materials");
	materialRegistry->registerMaterial((new Material(_R("craftgame:blocks/dirt")))->setTexture(CGE::instance->textureManager->loadTexture("res/blocks/dirt.png")));
	materialRegistry->registerMaterial((new Material(_R("craftgame:blocks/grass/top")))
		->setTexture(CGE::instance->textureManager->loadTexture("res/blocks/grass.png"))
		->setColor(glm::vec4(0, 0.7f, 0, 1))
		//->setSpecularMap(CGE::instance->textureManager->loadTextureRaw("res/blocks/grass-specular.png"))
	);
	materialRegistry->registerMaterial((new Material(_R("craftgame:blocks/grass/side")))->setTexture(CGE::instance->textureManager->loadTexture("res/blocks/dirt.png"))->set2Layer(
		(new SecondMaterial(_R("craftgame:blocks/grass/overlay")))
		->setTexture(CGE::instance->textureManager->loadTexture("res/blocks/grass_side.png"))
		->setColor(glm::vec4(0, 0.7f, 0, 1))
		//->setSpecularMap(CGE::instance->textureManager->loadTextureRaw("res/blocks/grass-specular.png"))
	));
	materialRegistry->registerMaterial((new Material(_R("craftgame:gold")))
		->setColor(Utils::fromHex(0xD18F1380))
		->setRoughness(.23f)
		->setF0(glm::vec3(1.00, 0.71, 0.29))
		->setNormalMap(CGE::instance->textureManager->loadTexture("res/blocks/noise-normal.png"))
		->setRefract(1.05f)
		->setMetal(true));

	materialRegistry->registerMaterial((new Material(_R("craftgame:blocks/stone")))->setTexture(CGE::instance->textureManager->loadTexture("res/blocks/stone.png")));

	materialRegistry->registerMaterial((new Material(_R("craftgame:blocks/cobblestone")))
		->setTexture(CGE::instance->textureManager->loadTexture("res/blocks/cobblestone.png"))
		//->setTexture(CGE::instance->textureManager->loadTexture("craftgame:noise4x4"))
		->setNormalMap(CGE::instance->textureManager->loadTexture("res/blocks/cobblestone-normal.png")));
	materialRegistry->registerMaterial((new Material(_R("craftgame:rose")))
		->setTexture(CGE::instance->textureManager->loadTexture("res/models/rose.png")));
	materialRegistry->registerMaterial((new Material(_R("craftgame:stick")))
		->setTexture(textureManager->loadTexture(_R("res/models/stick.png"), false)));
	
	
	materialRegistry->registerMaterial((new Material(_R("craftgame:yoyo")))
		->setTexture(textureManager->loadTexture(_R("res/models/yoyo.png"), true))
	->setRoughness(.21f)
	->setF0(glm::vec3(Utils::fromHex(0xff0024ff)))
	->setRefract(1.25f));
	materialRegistry->registerMaterial((new Material(_R("craftgame:blocks/container")))
		->setTexture(textureManager->loadTexture(_R("res/blocks/container.png"), true)));

	checkOpenGLError("standartGuiShader");
	standartGuiShader = new Shader("res/glsl/guiV.glsl", "res/glsl/guiF.glsl");
	standartGuiShader->compile();
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

	checkOpenGLError("maskShader");
	maskShader = new Shader("res/glsl/maskV.glsl", "res/glsl/maskF.glsl");
	maskShader->compile();
	maskShader->bindAttribute("position");
	maskShader->bindAttribute("texCoords");
	maskShader->link();
	maskShader->start();
	maskShader->loadInt("t", 0);
	maskShader->loadInt("texturing", 0);
	maskShader->loadVector("c", glm::vec4(1, 1, 1, 1));

	checkOpenGLError("guiShader");
	guiShader = standartGuiShader;
	blurShader = new BlurShader("res/glsl/blurV.glsl", "res/glsl/blurF.glsl");
	blurShader->compile();
	blurShader->bindAttribute("position");
	blurShader->bindAttribute("texCoords");
	blurShader->link();
	blurShader->start();
	blurShader->loadInt(guiShader->getUniform("useProjection"), 0);
	blurShader->loadVector("c", glm::vec4(1, 1, 1, 1));
	blurShader->loadInt(guiShader->getUniform("fb"), 0);
	blurShader->loadFloat(guiShader->getUniform("orientation"), 0);
	blurShader->loadInt("mask", 4);
	blurShader->loadInt("masking", 0);
	blurShader->setBlurRadius(13);
	blurShader->stop();

	checkOpenGLError("defaultStaticShader");
	defaultStaticShader = new StaticShader("res/glsl/staticV.glsl", "res/glsl/staticF.glsl");
	shadowDefaultStaticShader = new StaticShader("res/glsl/staticV.glsl", "res/glsl/staticF.glsl");
	shadowDefaultStaticShader->fgeometry = "res/glsl/shadowG.glsl";
	shadowDefaultStaticShader->defines["SHADOWS"] = 1;
	compileStaticShader(defaultStaticShader);
	compileStaticShader(shadowDefaultStaticShader);
	checkOpenGLError("skeletonShader");
	skeletonShader = new StaticShader("res/glsl/skeletonV.glsl", "res/glsl/staticF.glsl");
	shadowSkeletonShader = new StaticShader("res/glsl/skeletonV.glsl", "res/glsl/staticF.glsl");
	shadowSkeletonShader->fgeometry = "res/glsl/shadowG.glsl";
	shadowSkeletonShader->defines["SHADOWS"] = 1;
	compileSkeletonShader(skeletonShader);
	compileSkeletonShader(shadowSkeletonShader);

	checkOpenGLError("starShader");
	starShader = new Shader("res/glsl/starV.glsl", "res/glsl/starF.glsl");
	starShader->compile();
	starShader->bindAttribute("position");
	starShader->link();
	starShader->start();

	checkOpenGLError("particleShader");
	particleShader = new Shader("res/glsl/particleV.glsl", "res/glsl/particleF.glsl");
	particleShader->compile();
	particleShader->bindAttribute("position");
	particleShader->bindAttribute("uv");
	particleShader->bindAttribute("color");
	particleShader->bindAttribute("trans");
	particleShader->bindAttribute("col");
	particleShader->bindAttribute("uvs");
	particleShader->link();
	particleShader->start();
	particleShader->loadInt("tex", 0);
	particleShader->loadVector("c", glm::vec4(1.f));

	checkOpenGLError("particleShader2");
	particleShader2 = new Shader("res/glsl/particle2V.glsl", "res/glsl/particleF.glsl");
	particleShader2->compile();
	particleShader2->bindAttribute("position");
	particleShader2->bindAttribute("uv");
	particleShader2->bindAttribute("color");
	particleShader2->link();
	particleShader2->start();
	particleShader2->loadInt("tex", 0);
	particleShader2->loadVector("c", glm::vec4(1.f));

	shadowFB = nullptr;

	__createGBufferShader("res/glsl/gbufferF.glsl");
	//__createGBufferShader("res/glsl/greflectF.glsl");
	//__createGBufferShader("res/glsl/gssaoF.glsl");
	__createGBufferShader("res/glsl/gpostF.glsl");
	checkOpenGLError("Size specific (INIT)");
	sizeSpecific();

	checkOpenGLError("ShadowFB INIT");
	shadowFB = new Depthbuffer(2048, 2);

	checkOpenGLError("Gui shader start (INIT)");
	guiShader->start();
	displayGuiScreen(new GuiScreenLoad);
	CGE::instance->uiThread.push([&]()
	{
		CGE_setVSync(settings->getProperty<int>(_R("craftgame:graphics/maxFramerate")) == 0);
	});
	checkOpenGLError("Init end");
}
extern std::string _loading_errors;
void extensionsCheck()
{
	std::vector<std::vector<std::string>::iterator> unsupported;
	std::vector<std::string> extensions = {
		//"GL_ARB_bindless_texture"
	};
	for (std::vector<std::string>::iterator i = extensions.begin(); i != extensions.end(); i++)
	{
		if (!glewIsSupported(i->c_str()))
		{
			unsupported.push_back(i);
		}
	}
	if (!unsupported.empty())
	{
		WARN("Found unsupported OpenGL extensions:");
		std::string msg = _("gui.mm.oldgraphics.msg");
		for (size_t i = 0; i < unsupported.size(); i++)
		{
			msg += "\n   - " + *unsupported[i];
			WARN(std::string(" - ") + *unsupported[i]);
		}
		CGE::instance->displayGuiScreen(new GuiScreenMessageDialog(_("gui.mm.oldgraphics"), msg));
	}
}

#endif
//bool loaded = false;
void CGE::GameEngine::postInit() {
	try {
		//std::this_thread::sleep_for(std::chrono::seconds(10));
#ifndef SERVER
		logger->setThreadName("secondary gl thread");
		try {
			modelRegistry->registerModel("res/models/player.fbx", _R("craftgame:player"), false);
			modelRegistry->registerModel("res/models/playerg.fbx", _R("craftgame:playerg"), false);
		}
		catch (CraftgameException e) {
			logger->err(e.what());
		}
		CGE::instance->uiThread.push([&]()
		{
			if (!CGE::instance->guiScreens.empty()) {
				GuiScreenLoad* ls = dynamic_cast<GuiScreenLoad*>(CGE::instance->guiScreens.back().get());
				if (ls)
				{
					ls->setDisplayStringAsync(CGE::instance->currentLocale->localize("gui.load.models"));
				}
			}
		});

		/* Models */
		//modelRegistry->registerModel("res/models/torch.fbx", _R("craftgame:debug"));
		modelRegistry->registerModel("res/models/torch.obj", _R("craftgame:torch"));
		modelRegistry->registerModel("res/models/rose.obj", _R("craftgame:rose"));
		modelRegistry->registerModel("res/models/dimon.fbx", _R("craftgame:dimon"), false);

		CGE::instance->uiThread.push([&]()
		{
			if (!CGE::instance->guiScreens.empty()) {
				GuiScreenLoad* ls = dynamic_cast<GuiScreenLoad*>(CGE::instance->guiScreens.back().get());
				if (ls)
				{
					ls->setDisplayStringAsync(CGE::instance->currentLocale->localize("gui.load.fonts"));
				}
			}
		});

		/* CFonts */
		fontRegistry->registerCFont(new CFont("res/6563.ttf", _R("craftgame:futura")));
		fontRegistry->registerCFont(new CFont("res/futuralightc.otf", _R("craftgame:futuralight")));
		fontRegistry->registerCFont(new CFont("res/cbb.otf", _R("craftgame:comicbook")));
		fontRegistry->registerCFont(new CFont("res/inventory.otf", _R("inventory")));
		soundManager = new SoundManager;
		soundManager->initializeDevice(alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER));
		soundManager->registerSound(new Sound(_R("craftgame:pop"), "res/sound/pop.ogg"));
		//soundManager->registerSound(new Sound(_R("craftgame:azaza"), "res/MFP.ogg"));
		//soundManager->playSoundEffect(new SoundEffect(_R("craftgame:azaza"), glm::vec3(0, 0, 0), .5f, 1.f, true));

		if (!gameProfile)
		{
			if (settings->getPropertyString("gameprofile/username").empty()) {
				std::condition_variable cv;
				std::mutex m;
				std::unique_lock<std::mutex> lock(m);
				bool ok = false;
				uiThread.push([&]()
				{
					displayGuiScreen(new GuiScreenRegister([&](const std::string u)
					{
						settings->setProperty("gameprofile/username", u);
						settings->apply();
						ok = true;
						cv.notify_all();
					}));
				});
				while (!ok)
					cv.wait(lock);
			}
			gameProfile = new GameProfile(settings->getPropertyString("gameprofile/username"));
			INFO(std::string("Setting up in-game user ") + gameProfile->getUsername());
		}

		CGE::instance->uiThread.push([&]()
		{
			if (!CGE::instance->guiScreens.empty()) {
				GuiScreenLoad* ls = dynamic_cast<GuiScreenLoad*>(CGE::instance->guiScreens.back().get());
				if (ls)
				{
					ls->setDisplayStringAsync(CGE::instance->currentLocale->localize("gui.load.blocks"));
				}
			}
		});
#endif

		Blocks::init();

#ifndef SERVER
		CGE::instance->uiThread.push([&]()
		{
			if (!CGE::instance->guiScreens.empty()) {
				GuiScreenLoad* ls = dynamic_cast<GuiScreenLoad*>(CGE::instance->guiScreens.back().get());
				if (ls)
				{
					ls->setDisplayStringAsync(CGE::instance->currentLocale->localize("gui.load.items"));
				}
			}
		});
#endif

		Items::init();

#ifndef SERVER
		CGE::instance->uiThread.push([&]()
		{
			if (!CGE::instance->guiScreens.empty()) {
				GuiScreenLoad* ls = dynamic_cast<GuiScreenLoad*>(CGE::instance->guiScreens.back().get());
				if (ls)
				{
					ls->setDisplayStringAsync(CGE::instance->currentLocale->localize("gui.load.python"));
				}
			}
		});
#endif
		PyImport_AppendInittab("cge", PyInit_cge);
		PyImport_AppendInittab("logger", PyInit_logger);
		//PyImport_AppendInittab("hook", PyInit_hook);

		Py_Initialize();



		INFO("Loading mods in mods/ folder");

		Dir dd = Dir("mods");
		if (!dd.isDir())
			dd.mkdir();
		std::vector<File> m = dd.list();
		m.reserve(m.size());

		PyObject* sysPath = PySys_GetObject("path");
		PyObject* modsPath = PyUnicode_FromString("mods");
		PyList_Insert(sysPath, 0, modsPath);
		python::import("logger");
		python::import("cge");


		//python::import("hook");
		for (File& file : m) {
			if (file.isDir())
			{
				try {
					boost::property_tree::ptree pt;
					std::string _path = file.path + "/package.json";
					std::ifstream fis(_path);
					if (fis.good()) {
						boost::property_tree::read_json(fis, pt);
						fis.close();
						std::string modPathPrefix = file.path + "/";
						{
							PyObject* modsPath = PyUnicode_FromString(modPathPrefix.c_str());
							PyList_Insert(sysPath, 0, modsPath);
						}
						std::string package_name = pt.get_child("name").get_value<std::string>();
						Resource::registerIStreamProvider(package_name, new DirStreamProvider(file.path));

						for (boost::property_tree::ptree::value_type& v : pt.get_child("mods"))
						{
							try {
								std::string modid = v.second.get_child("modid").get_value<std::string>();

								if (modManager->getModById(modid))
									throw std::runtime_error(modid + " mod already exists");
								auto c = v.second.get_child_optional("side");
								std::string side = "both";
								if (c)
								{
									side = c->get_value<std::string>();
								}
#ifdef SERVER
								std::string cs = "server";
#else
								std::string cs = "client";
#endif
								PythonMod* mod = new PythonMod(bpy::object());
								mod->name = v.second.get_child("name").get_value<std::string>();

#ifndef SERVER
								CGE::instance->uiThread.push([&]()
								{
									if (!CGE::instance->guiScreens.empty()) {
										GuiScreenLoad* ls = dynamic_cast<GuiScreenLoad*>(CGE::instance->guiScreens.back().get());
										if (ls)
										{
											ls->setDisplayStringAsync(CGE::instance->currentLocale->localize(CGE::instance->currentLocale->localize("gui.load.fmod", spair("name", mod->name))));
										}
									}
								});
#endif
								mod->modid = modid;
								mod->version = v.second.get_child("version").get_value<std::string>();
								mod->file = modPathPrefix + modid + ".py";
								modManager->mods.push_back(mod);
								boost::python::object b = python::import(modid.c_str());
								mod->handle.attr("init")();
							} catch (std::exception e)
							{
#ifdef SERVER
								logger->err(file.name() + ": " + std::string(e.what()));
#else
								_loading_errors += "\n\n" + file.name() + ": " + std::string(e.what());
#endif
							} catch (boost::python::error_already_set e)
							{
#ifdef SERVER
								logger->err(file.name() + ": " + Utils::pythonErrorString());
#else
								_loading_errors += "\n\n" + file.name() + ": " + Utils::pythonErrorString();
#endif
							}
						}
					}
				} catch (std::exception e) {
#ifdef SERVER
					logger->err(file.name() + "/package.json: " + std::string(e.what()));
#else
					_loading_errors += "\n\n" + file.name() + "/package.json: " + std::string(e.what());
#endif
				}
			} else if (file.isFile()) {
				if (Parsing::endsWith(file.path, ".cgx")) {
					try {
						BinaryInputStream fis(file.path);
						byte s;
						fis >> s;
						if (s == 0x3c)
						{
							std::string package_name = fis.readString();
							std::string version = fis.readString();
							ByteBuffer c;
							size_t size;
							fis >> size;
							c.reserve(size);
							fis.std::ifstream::read(c.get(), size);
							c.setSize(size);
							ByteBuffer unc;
							Utils::decompress(c, unc);
							Resource::registerIStreamProvider(package_name, new BufferedStreamProvider(unc));

							size_t count;
							fis >> count;
							for (size_t i = 0; i < count; i++)
							{
								try {
									std::string name = fis.readString();
									std::string modid = fis.readString();
									std::string version = fis.readString();
									int cge = fis.read<int>();
									byte side = fis.read<byte>();

									if (modManager->getModById(modid))
										throw std::runtime_error(modid + " mod already exists");
									
#ifdef SERVER
									byte cs = 2;
#else
									byte cs = 1;
#endif
									if (side == 0 || side == cs) {
										PythonMod* mod = new PythonMod(bpy::object());
										mod->name = name;
#ifndef SERVER
										CGE::instance->uiThread.push([&]()
										{
											if (!CGE::instance->guiScreens.empty()) {
												GuiScreenLoad* ls = dynamic_cast<GuiScreenLoad*>(CGE::instance->guiScreens.back().get());
												if (ls)
												{
													ls->setDisplayStringAsync(CGE::instance->currentLocale->localize(CGE::instance->currentLocale->localize("gui.load.mod", spair("name", mod->name))));
												}
											}
										});
#endif
										mod->modid = modid;
										mod->version = version;
										mod->file = file.path;
										modManager->mods.push_back(mod);
										Resource r(package_name + ":" + modid + ".pyc");
										std::shared_ptr<std::istream> fis = r.open();
										fis->seekg(0, std::ios::end);
										size_t size = fis->tellg();
										fis->seekg(0, std::ios::beg);
										char* tmp = new char[size];
										fis->read(tmp, size); 
										PyObject* o = PyMarshal_ReadObjectFromString(tmp, size);
										PyObject *module = PyImport_ExecCodeModule(package_name.c_str(), o);
										mod->handle.attr("init")();
										delete[] tmp;
									}
								}
								catch (std::exception e)
								{
#ifdef SERVER
									logger->err(file.name() + ": " + std::string(e.what()));
#else
									_loading_errors += "\n\n" + file.name() + ": " + std::string(e.what());
#endif
								}
								catch (boost::python::error_already_set e)
								{
#ifdef SERVER
									logger->err(file.name() + ": " + Utils::pythonErrorString());
#else
									_loading_errors += "\n\n" + file.name() + ": " + Utils::pythonErrorString();
#endif
								}
							}
						}
						fis.close();
					}
					catch (std::exception e) {
#ifdef SERVER
						logger->err(file.name() + ":" + std::string(e.what()));
#else
						_loading_errors += "\n\n" + file.name() + ": " + std::string(e.what());
#endif
					}
				}
			}
		}
		modManager->currentFile.clear();
		Py_DECREF(sysPath);
		Py_DECREF(modsPath);
		if (m.empty())
		{
			// Retrieve the main module.
			python::object main = python::import("__main__");

			// Retrieve the main module's namespace
			python::object global(main.attr("__dict__"));
			boost::python::exec("import logger\nlogger.info(\"No mods found\")", global, global);
		}
		modManager->initiated = true;


#ifndef SERVER
		CGE::instance->uiThread.push([&]()
		{
			if (!CGE::instance->guiScreens.empty()) {
				GuiScreenLoad* ls = dynamic_cast<GuiScreenLoad*>(CGE::instance->guiScreens.back().get());
				if (ls)
				{
					ls->setDisplayStringAsync(CGE::instance->currentLocale->localize("gui.load.particle_ta"));
				}
			}
		});
		{
			TextureAtlas::Generator gen;

			gen.addFragment(CGE::instance->imageLoader->load(_R("res/particle/p1.png")));
			gen.addFragment(CGE::instance->imageLoader->load(_R("res/particle/p2.png")));
			gen.addFragment(CGE::instance->imageLoader->load(_R("res/particle/p3.png")));

			size_t side;
			Image* atlas = gen.generate(side);
			
			uiThread.push([&, atlas, side]()
			{
				taParticles = TextureAtlas(CGE::instance->textureManager->loadTexture(atlas, _R("ta/particles")), side, side);
			});
		}

#endif
		//-----------------
		
#ifndef SERVER
		CGE::instance->uiThread.push([&]() {
			boost::shared_ptr<GuiScreen> d = guiScreens[0];
			d->animations.push_back(new AnimationGSL(d.get()));
			d->animations[0]->frame = 8.f;
			d->animatedClose(d->animations[0]);
			guiScreens.clear();
			displayGuiScreen(new GuiScreenMainMenu());
			displayGuiScreen(d);
			extensionsCheck();
			displayGuiScreen(new GuiScreenMessageDialog("We've updated privacy policy", "On May 24th we updated our Privacy Policy to ensure it meets all requirements of the European GDPR directive, which involves data privacy laws. It makes clear how and why we collect, process, retain and delete data, and which rights you have as a Craftgame player."));
			if (!_loading_errors.empty()) {
				displayGuiScreen(new GuiScreenMessageDialog("Errors during loading", std::string("Details:") + _loading_errors));
				_loading_errors.clear();
			}

			CGE_EVENT("startup", , []()
			{

			});
			//loaded = true;
			//displayGuiScreen(new GuiScreenMessageDialog("", rand() % 2 ? "1" : "Не 1"));
		});
#endif
	} catch (std::exception& e)
	{
		logger->err("An error has occurred while postinit");
		logger->err(e.what());
		exit(0);
	}
}
#ifndef SERVER
void CGE::GameEngine::focusGain()
{
	CGE::instance->focus = true;
	if (CGE::instance->thePlayer)
		CGE::instance->thePlayer->updateState(CGE::instance->thePlayer->state & ~(1 << 2));
}
void CGE::GameEngine::focusLost()
{
	CGE::instance->focus = false;
	if (CGE::instance->thePlayer)
		CGE::instance->thePlayer->updateState(CGE::instance->thePlayer->state | (1 << 2));
}
void CGE::GameEngine::onResize(int _width, int _height) {
	width = _width;
	height = _height;
	sizeSpecific();
	for (std::deque<boost::shared_ptr<GuiScreen>>::iterator i = guiScreens.begin(); i != guiScreens.end(); i++) {
		(*i)->onLayoutChange(0, 0, _width, _height);
	}
}
void processInput()
{
	if (CGE::instance->guiScreens.empty()) {
		/*
		if (Keyboard::isKeyDown(Keyboard::LButton) || Keyboard::isKeyDown(Keyboard::RButton))
		{
		if (kek) {
		kek = false;
		float size = Keyboard::isKeyDown(Keyboard::LButton) ? 0.2f : 1.3f;
		size += (rand() % 400) / 100.f;

		glm::vec4 k = glm::inverse(CGE::instance->camera->createViewMatrix()) * glm::inverse(CGE::instance->projection) * glm::vec4(0, 0, 0, 1);
		std::ofstream fos("kek.txt", std::ios::app);
		fos << "color = star(color, ray.xyz, vec3(" << k.x << ", " << k.y << ", " << k.z << "), " << size << ");" << std::endl;
		fos.close();
		CGE::instance->skyboxShader->loadVector(std::string("stars[") + std::to_string(id) + "]", glm::vec3(k));
		CGE::instance->skyboxShader->loadFloat(std::string("starsSize[") + std::to_string(id++) + "]", size);
		CGE::instance->skyboxShader->loadInt("starsCount", id);
		}
		} else
		{
		kek = true;
		}*/

		//CGE::instance->camera->yaw = -CGE::instance->thePlayer->pitch;
		//CGE::instance->camera->pitch = -CGE::instance->thePlayer->yaw;

		glm::vec3 motion;
		CGE::instance->cursor = false;
		const float speed = 1.f;
		switch (CGE::instance->joystick) {
		case 0:
		{
			int xc = CGE::instance->width / 2;
			int yc = CGE::instance->height / 2;
			int mouseX, mouseY;
			OS::getCursorPos(mouseX, mouseY);
			CGE::instance->camera->pitch = CGE::instance->thePlayer->pitch + float(yc - mouseY) * 0.1f;
			CGE::instance->camera->yaw = CGE::instance->thePlayer->yawHead + float(xc - mouseX) * 0.1f;
			CGE::instance->camera->pitch = glm::clamp(CGE::instance->camera->pitch, -90.f, 90.f);
			OS::setCursorPos(xc, yc);
		}
		if (Keyboard::isKeyDown(Keyboard::W)) {
			motion.z -= speed * sin((CGE::instance->camera->yaw + 90.f) * M_PI / 180.f);
			motion.x += speed * cos((CGE::instance->camera->yaw + 90.f) * M_PI / 180.f);
		}
		if (Keyboard::isKeyDown(Keyboard::S)) {
			motion.z -= speed * sin((CGE::instance->camera->yaw - 90.f) * M_PI / 180.f);
			motion.x += speed * cos((CGE::instance->camera->yaw - 90.f) * M_PI / 180.f);
		}
		if (Keyboard::isKeyDown(Keyboard::D)) {
			motion.z -= speed * sin((CGE::instance->camera->yaw) * M_PI / 180.f);
			motion.x += speed * cos((CGE::instance->camera->yaw) * M_PI / 180.f);
		}
		if (Keyboard::isKeyDown(Keyboard::A)) {
			motion.z -= speed * sin((CGE::instance->camera->yaw + 180.f) * M_PI / 180.f);
			motion.x += speed * cos((CGE::instance->camera->yaw + 180.f) * M_PI / 180.f);
		}
		motion = glm::normalize(motion) * speed;
		if (motion.x != motion.x)
			motion.x = 0;
		if (motion.y != motion.y)
			motion.y = 0;
		if (motion.z != motion.z)
			motion.z = 0;
		if (Keyboard::isKeyDown(Keyboard::Space) && (CGE::instance->thePlayer->isFlying() || CGE::instance->thePlayer->isOnGround())) {
			motion.y += (CGE::instance->thePlayer->isFlying() ? speed : 7.f) / 1.1f;
		}
		if (Keyboard::isKeyDown(Keyboard::LShift)) {
			motion.y -= speed / 1.1f;
		}
		break;
		case 1:
			CGE::instance->camera->pitch = CGE::instance->thePlayer->pitch + Joystick::stick2.y * CGE::instance->millis * 62 * -3;
			CGE::instance->camera->yaw = CGE::instance->thePlayer->yawHead + Joystick::stick2.x * CGE::instance->millis * 62 * -3;
			CGE::instance->camera->pitch = glm::clamp(CGE::instance->camera->pitch, -90.f, 90.f);

			motion.z += speed * sin((CGE::instance->camera->yaw + 90.f) * M_PI / 180.f) * Joystick::stick1.y;
			motion.x -= speed * cos((CGE::instance->camera->yaw + 90.f) * M_PI / 180.f) * Joystick::stick1.y;

			motion.z += speed * sin((CGE::instance->camera->yaw + 180.f) * M_PI / 180.f) * Joystick::stick1.x;
			motion.x -= speed * cos((CGE::instance->camera->yaw + 180.f) * M_PI / 180.f) * Joystick::stick1.x;

			if (Joystick::getKeyState(Joystick::A) && (CGE::instance->thePlayer->isFlying() || CGE::instance->thePlayer->isOnGround())) {
				motion.y += (CGE::instance->thePlayer->isFlying() ? speed : 7.f) / 1.1f;
			}
			if (Joystick::getKeyState(Joystick::L2)) {
				motion.y -= speed / 1.1f;
			}

			break;
		}
		static glm::vec3 prevMotion;

		//CGE::instance->thePlayer->performCollisionChecks();
		if (motion != prevMotion)
		{
			//CGE::instance->thePlayer->applyMotion(motion);
			prevMotion = motion;
			ts<IServer>::r(CGE::instance->server)->sendPacket(new PForce(motion));
		}
		if (CGE::instance->camera->yaw != CGE::instance->thePlayer->yawHead || CGE::instance->camera->pitch != CGE::instance->thePlayer->pitch)
		{
			CGE::instance->thePlayer->setLook(CGE::instance->camera->yaw, CGE::instance->camera->pitch);
			ts<IServer>::r(CGE::instance->server)->sendPacket(new PEntityLook(CGE::instance->thePlayer));
		} else
		{
			CGE::instance->thePlayer->setLook(CGE::instance->camera->yaw, CGE::instance->camera->pitch);
		}

		if (Joystick::getKeyState(Joystick::L1) || Keyboard::isKeyDown(Keyboard::LButton)) {
			CGE::instance->uiThread.push(PlayerController::leftClick);
		}
		if (Joystick::getKeyState(Joystick::R1) || Keyboard::isKeyDown(Keyboard::RButton)) {
			CGE::instance->uiThread.push(PlayerController::rightClick);
		}
	} else
	{
		CGE::instance->camera->pitch = CGE::instance->thePlayer->pitch;
		CGE::instance->camera->yaw = CGE::instance->thePlayer->yawHead;
	}
}
void CGE::GameEngine::sizeSpecific() {
	if (width && height) {
		renderer->changeViewport(0, 0, width, height);

		if (mainFB) {
			delete mainFB;
			delete secFB;
			delete thirdFB;
			delete maskFB;
			delete gBuffer;
		}
		if (glewInitiated) {
			mainFB = new Framebuffer(width, height);
			secFB = new SecFramebuffer(width, height);
			thirdFB = new SecFramebuffer(width, height);
			maskFB = new MaskFramebuffer(width, height);
			gBuffer = new GBuffer(width, height);

			mainFB->bind(width, height);

			projection = glm::perspective(glm::radians(75.f), float(width) / float(height), 0.05f, 500.f);
			projection2 = glm::perspective(glm::radians(75.f), 1.f, 0.05f, 500.f);

			for (size_t i = 0; i < gbufferShader.size(); i++) {
				gbufferShader[i]->use();
				gbufferShader[i]->loadVector("screen", glm::vec2(width, height));
			}

			defaultStaticShader->use();
			defaultStaticShader->loadMatrix("projection", projection);
			skeletonShader->use();
			skeletonShader->loadMatrix("projection", projection);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, CGE::instance->maskFB->texture);
			glActiveTexture(GL_TEXTURE0);
		}
	}
}
GuiScreenMessageDialog* _connectionDialog = nullptr;
// bool opengl_error = false;
void CGE::GameEngine::render(int mouseX, int mouseY) {
	auto renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	static bool overlayCheck = true;
	static bool drag = true;
	size_t transform = guiShader->getUniform("transform");
	static size_t background = textureManager->loadTexture("res/blocks/dirt.png");
	static size_t cobblestone = textureManager->loadTexture("res/blocks/cobblestone.png");
	size_t uv = CGE::instance->guiShader->getUniform("uv");
	size_t uv2 = CGE::instance->guiShader->getUniform("uv2");
	static size_t scales = CGE::instance->blurShader->getUniform("scales");
	static size_t cursort = CGE::instance->textureManager->loadTexture("res/cursor.png", true);
	static CFont* defaultCFont = CGE::instance->fontRegistry->getCFont(_R("craftgame:default"));

	if (t < time(0)) {
		t = time(0);
		fpss = fps;
		fps = 0;
	}
	while (!uiThread.empty()) {
		uiThread.front()();
		uiThread.pop();
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	defaultStaticShader->use();
	camera->upload();
	skeletonShader->startNonG();
	camera->upload();

	skeletonShader->Shader::stop();
	blurShader->start();
	blurShader->loadVector(scales, glm::vec2(1.f / float(width), 1.f / float(height)));
	
	//guiShader->start();
	mainFB->bind(width, height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GameEngine::mouseX = mouseX;
	GameEngine::mouseY = mouseY;

	gbufferShader[1]->start();
	gbufferShader[1]->loadInt("bskybox", CGE::instance->server ? 1 : 0);

	if (!_connectionDialog) {
		ts<IServer>::r l(server); // Знаю я ваши эти потоки, ага
		if (thePlayer && thePlayer->worldObj)
		{
			{
				float ff = cos((camera->pitch) / 180.f * glm::pi<float>());
				float y = sin((camera->pitch) / 180.f * glm::pi<float>()) * 0.1f;
				float x = sin((camera->yaw + 180) / 180.f * glm::pi<float>()) * 0.1f * ff;
				float z = cos((camera->yaw + 180) / 180.f * glm::pi<float>()) * 0.1f * ff;
				soundManager->setListener(camera->pos, glm::normalize(glm::vec3(x, y, z)));
			}
			//checkOpenGLError("Process input");
			processInput();
			//checkOpenGLError("ShadowFB bind");
			if (thePlayer->worldObj->calculateSunPos().y > 0.f) {
				shadowFB->bind();
				glEnable(GL_DEPTH_TEST);
				glClear(GL_DEPTH_BUFFER_BIT);
				renderer->renderType = GameRenderer::RENDER_FIRST | GameRenderer::RENDER_SHADOW;

				StaticShader* st1 = defaultStaticShader;
				StaticShader* st2 = skeletonShader;

				defaultStaticShader = shadowDefaultStaticShader;
				skeletonShader = shadowSkeletonShader;
				defaultStaticShader->start();
				//checkOpenGLError("Shadow rendering");
				renderer->render(1);

				defaultStaticShader = st1;
				skeletonShader = st2;
				renderer->renderType = GameRenderer::RENDER_LAST | GameRenderer::RENDER_DEFAULT;
			} else {			
				renderer->renderType = GameRenderer::RENDER_FIRST | GameRenderer::RENDER_DEFAULT;
			}
			//checkOpenGLError("MainFB bind");
			defaultStaticShader->start();

			mainFB->bind(width, height);
			//checkOpenGLError("Default rendering");
			renderer->render(0);
			guiShader->start();
			//checkOpenGLError("Rendering ingame UI");
			renderer->drawIngameGui();

			RemoteServer* r = dynamic_cast<RemoteServer*>(l.get());
			if (time(0) - r->net->lastPacket >= 30)
			{
				disconnect("cge.error.timeout");
			}
			//checkOpenGLError("Ingame rendering done");
		}
		if (thePlayer && thePlayer->worldObj && guiScreens.empty()) {
			Joystick::setJoystickHandler(Joystick::LEFT, Joystick::JoystickButton(1, _("game.change_item"), [&]() {
				if (thePlayer->getItemIndex() == 0) {
					thePlayer->setItemIndex(8);
				} else
				{
					thePlayer->setItemIndex(thePlayer->getItemIndex() - 1);
				}
			}));
			Joystick::setJoystickHandler(Joystick::RIGHT, Joystick::JoystickButton(1, _("game.change_item"), [&]() {
				if (thePlayer->getItemIndex() == 8) {
					thePlayer->setItemIndex(0);
				}
				else
				{
					thePlayer->setItemIndex(thePlayer->getItemIndex() + 1);
				}
			}));
			Joystick::setJoystickHandler(Joystick::L1, Joystick::JoystickButton(1, _("game.break"), []() {}));
			Joystick::setJoystickHandler(Joystick::R1, Joystick::JoystickButton(1, _("game.place"), []() {}));
			Joystick::setJoystickHandler(Joystick::L2, Joystick::JoystickButton(1, _("game.sneak"), []() {}));
			Joystick::setJoystickHandler(Joystick::A, Joystick::JoystickButton(1, _("game.jump"), []()
			{				
				_jump();
			}));
			//Joystick::setJoystickHandler(Joystick::Y, Joystick::JoystickButton(1, _("game.inventory"), []() {}));
			Joystick::setJoystickHandler(Joystick::START, Joystick::JoystickButton(1, _("game.pause"), []() {
				CGE::instance->displayGuiScreen(new GuiScreenPause);
			}));
		}
	}
	guiShader->start();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderer->color = glm::vec4(1.f);
	if (!_connectionDialog && !(Keyboard::isKeyDown(Keyboard::LControl) && Keyboard::isKeyDown(Keyboard::LAlt) && Keyboard::isKeyDown(Keyboard::C)))
	{
		int offset = 0;
		if (chat->full) {
			offset -= chat->scroll;
		}
		float _a = 1.f;
		for (std::deque<boost::shared_ptr<GuiScreen>>::iterator i = guiScreens.begin(); i != guiScreens.end(); i++)
		{
			if (GuiScreenChat* c = dynamic_cast<GuiScreenChat*>(i->get()))
			{
				if (!c->f->getText().empty() && c->f->getText()[0] == '/')
				{
					_a = 0.2f;
				}
				break;
			}
		}
		// CHAT
		for (Chat::mqueue::iterator i = chat->chatQueue.begin(); i != chat->chatQueue.end(); i++)
		{
			std::vector<std::string> list = defaultCFont->trimStringToMultiline(i->message, 696, 15);
			float alpha = _a;
			
			if (!chat->full) {
				long long l_millis = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
				if (l_millis - 10000 > i->timestamp)
				{
					alpha -= float(l_millis - 10000 - i->timestamp) / 1000.f;
				}
			}
			if (alpha > 0) {
				FontStyle s;
				s.color.a = alpha;
				s.ts = TextStyle::SHADOW;
				s.size = 15;

				for (size_t j = 0; j < list.size(); j++) {
					int y = height - ((offset + list.size() - j - 1) * 21 + 70);
					if (y - 200 > 0 && y < height) {
						renderer->setTexturingMode(0);
						renderer->setColor(glm::vec4(0, 0, 0, 0.5 * alpha));
						renderer->drawRect(0, y, 704, 21);
						renderer->drawString(3, y + 1, list[j], Align::LEFT, s, true);
					}
				}
				offset += list.size();
			}
		}
		int y = height - 70;
		chat->height = (y + chat->scroll) - (offset) * 21;
		while (chat->chatQueue.size() > 100)
		{
			chat->chatQueue.pop_back();
		}
		if (!focus && guiScreens.empty())
		{
			displayGuiScreen(new GuiScreenPause);
		}
	}
	renderer->setMaskingMode(1);
	int jg = 0;

	for (std::vector<GuiWindow*>::iterator i = guiWindows.begin(); i != guiWindows.end(); i++) {
		GuiWindow* s = *i;
		glm::vec4 v = s->getTransform() * glm::vec4(1, 1, 1, 1);

		int x = int((v.x - 1) / 2.f * CGE::instance->width);
		int y = int((v.y - 1) / -2.f * CGE::instance->height);
		if (x <= mouseX && x + s->width >= mouseX && y <= mouseY && y + s->height >= mouseY) {
			jg |= OVERLAYED;
		}
	}
	for (std::deque<boost::shared_ptr<GuiScreen>>::iterator i = guiScreens.begin(); i != guiScreens.end(); i++) {
		GuiScreen* s = i->get();
		try {
			renderer->color = glm::vec4(1);
			CGE_EVENT_CHECK {
				CGE_EVENT("render_screen", CGE_P("gui", static_cast<Gui*>(s))CGE_P("name", Utils::classname(s)), [&]()
				{
				s->GuiContainer::render((guiScreens.end() - 1 == i ? RenderFlags::STANDART : (RenderFlags::STANDART | RenderFlags::OVERLAYED)) | jg, mouseX, mouseY);
					CGE_EVENT("render_screen_bg", CGE_P("gui", static_cast<Gui*>(s))CGE_P("name", Utils::classname(s)), [&]()
					{
						s->render((guiScreens.end() - 1 == i ? RenderFlags::STANDART : (RenderFlags::STANDART | RenderFlags::OVERLAYED)) | jg, mouseX, mouseY);
					});
					CGE_EVENT("render_screen_wg", CGE_P("gui", static_cast<Gui*>(s))CGE_P("name", Utils::classname(s)), [&]()
					{
						s->renderWidgets((guiScreens.end() - 1 == i ? RenderFlags::STANDART : (RenderFlags::STANDART | RenderFlags::OVERLAYED)) | jg, mouseX, mouseY);
					});
				});
			} else
			{
				s->GuiContainer::render((guiScreens.end() - 1 == i ? RenderFlags::STANDART : (RenderFlags::STANDART | RenderFlags::OVERLAYED)) | jg, mouseX, mouseY);
				s->render((guiScreens.end() - 1 == i ? RenderFlags::STANDART : (RenderFlags::STANDART | RenderFlags::OVERLAYED)) | jg, mouseX, mouseY);
				s->renderWidgets((guiScreens.end() - 1 == i ? RenderFlags::STANDART : (RenderFlags::STANDART | RenderFlags::OVERLAYED)) | jg, mouseX, mouseY);
			}
		} catch (boost::python::error_already_set e)
		{
			PyErr_Print();
		}
	}
	renderer->color = glm::vec4(1);
	for (std::vector<GuiWindow*>::iterator i = guiWindows.begin(); i != guiWindows.end(); i++) {
		GuiWindow* s = *i;
		s->render(guiWindows.end() - 1 == i ? RenderFlags::STANDART : (RenderFlags::STANDART | RenderFlags::OVERLAYED), mouseX, mouseY);
	}
	renderer->transform = glm::mat4(1.0);
	renderer->color = glm::vec4(1);

	CGE::instance->maskFB->begin();
	CGE::instance->guiShader = CGE::instance->maskShader;
	CGE::instance->guiShader->start();
	CGE::instance->renderer->drawRect(0, 0, width, height);
	CGE::instance->guiShader = CGE::instance->standartGuiShader;
	CGE::instance->guiShader->start();
	CGE::instance->maskFB->end();

	while (!_postRender.empty()) {
		_postRender.front()();
		_postRender.pop();
	}
	

	if (guiScreens.size() > 1 || (guiScreens.size() > 0 && CGE::instance->thePlayer)) {
		GuiScreenMessageDialog* d = dynamic_cast<GuiScreenMessageDialog*>(guiScreens.back().get());
		if (!d || !d->widgets.empty()) {
			Joystick::setJoystickHandler(Joystick::B, Joystick::JoystickButton(1, _("gui.back"), [&]() {
				guiScreens.back()->close();
			}));
		}
	}

	renderer->setMaskingMode(0);
	renderer->transform = glm::mat4(1.0);
	static bool* debug = settings->getPropertyPointer<bool>(_R("craftgame:debug"));
	if ((fpss <= 10 && fpss > 0) || *debug) {
		renderer->color = glm::vec4(1);
#ifdef WINDOWS
		glm::vec4 c = glm::mix(glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), glm::min(float(fpss) / 60.f, 1.f));
#else
		glm::vec4 c = glm::mix(glm::vec4(1, 0, 0, 1), glm::vec4(0, 1, 0, 1), glm::min(float(fpss) / 60.f, 1.f));
#endif
		renderer->drawString(2, 2, std::to_string(fpss) + std::string(" FPS"), Align::LEFT, TextStyle::SHADOW, c);
		renderer->drawString(90, 2, std::string("[") + std::to_string(millis) + "]", Align::LEFT, TextStyle::SHADOW, glm::vec4(0.5f, 0.5f, 0.5f, 1.f));
		if (fpss <= 10) {
			c.a = (cos(float(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 1000) / 1000.f * 20 / 3.14) + 1) / 2;
			renderer->drawString(80, 2, "!", Align::LEFT, TextStyle::SHADOW, c);
		}
	}
	if (server && *debug)
	{
		/*
		renderer->drawString(width - 120, 2, std::string("in"), Align::RIGHT, TextStyle::SHADOW);
		renderer->drawString(width - 75, 2, Utils::bytesFormat(d_srv.in_bytes) + "/s", Align::CENTER, TextStyle::SHADOW);
		renderer->drawString(width - 2, 2, std::to_string(d_srv.in_packets), Align::RIGHT, TextStyle::SHADOW);

		renderer->drawString(width - 120, 15, std::string("out"), Align::RIGHT, TextStyle::SHADOW);
		renderer->drawString(width - 75, 15, Utils::bytesFormat(d_srv.out_bytes) + "/s", Align::CENTER, TextStyle::SHADOW);
		renderer->drawString(width - 2, 15, std::to_string(d_srv.out_packets), Align::RIGHT, TextStyle::SHADOW);
		*/
		size_t yOffset = 1;
		for (std::vector<DebugInfo>::iterator i = debugInfo.begin(); i != debugInfo.end(); ++i)
		{
			renderer->drawString(2, yOffset * 17, i->key, Align::LEFT, TextStyle::SHADOW, glm::vec4(0.6f, 0.6f, 0.6f, .8f));
			float d = (i->highlightAnimation * 0.3f + 0.7f);
			renderer->drawString(8 + defaultCFont->length(i->key), yOffset * 17, i->value, Align::LEFT, TextStyle::SHADOW, glm::vec4(d, d, d, 1.f));
			i->highlightAnimation -= millis * 2;
			if (i->highlightAnimation < 0)
			{
				i->highlightAnimation = 0;
			}
			yOffset++;
		}
	}
	static bool* net_debug = settings->getPropertyPointer<bool>(_R("craftgame:net_debug"));
	if (*net_debug && guiScreens.empty() && CGE::instance->server)
	{
		static glm::vec4 color1 = Utils::fromHex(0x42525fff);
		static glm::vec4 color2 = Utils::fromHex(0x42525f00);
		static glm::vec4 text = Utils::fromHex(0xe2f2ffff);
		CGE::instance->renderer->setTexturingMode(3);
		CGE::instance->guiShader->loadVector("colors[0]", color2);
		CGE::instance->guiShader->loadVector("colors[1]", color1);
		CGE::instance->guiShader->loadVector("colors[2]", color2);
		CGE::instance->guiShader->loadVector("colors[3]", color1);
		static CFont* f = CGE::instance->fontRegistry->getCFont(_R("craftgame:futuralight"));
		int delay = -1;
		GuiList* fx = RemoteServer::playerList.get();
		for (GuiList::Widgets::iterator i = fx->widgets.begin(); i != fx->widgets.end(); ++i)
		{
			PlayerListEntry* item = dynamic_cast<PlayerListEntry*>((*i).get());
			if (item->name == gameProfile->getUsername())
			{
				delay = item->ping;
			}
		}
		struct kek
		{
			std::string name;
			std::string val1;
			std::string val2;
		};

		ts<IServer>::r lock = CGE::instance->server;
		if (CGE::instance->server) {
			RemoteServer* r = dynamic_cast<RemoteServer*>(lock.get());
			size_t in, out;
			r->net->getBPS(in, out);
			std::vector<kek> ozozo = {
				{
					_("gui.net_debug.fps_delay"),
					std::to_string(int(1.f / CGE::instance->millis)),
					std::to_string(delay)
				},
				{
					_("gui.net_debug.io"),
					Utils::bytesFormat(in) + "/s",
					Utils::bytesFormat(out) + "/s"
				},
				{
					_("gui.net_debug.loss"),
					std::to_string(r->net->p_loss_in),
					std::to_string(r->net->p_loss_out)
				}
			};

			CGE::instance->renderer->drawRect(width - 220, 20, 220, ozozo.size() * 21 + 6);
			size_t c = 0;
			for (kek& item : ozozo) {
				CGE::instance->renderer->drawString(width - 140, 26 + c * 21, item.name, Align::RIGHT, TextStyle::SHADOW, text, 14, f);
				CGE::instance->renderer->drawString(width - 70, 26 + c * 21, item.val1, Align::RIGHT, TextStyle::SHADOW, glm::vec4(1.f), 14);
				CGE::instance->renderer->drawString(width - 2, 26 + c * 21, item.val2, Align::RIGHT, TextStyle::SHADOW, glm::vec4(1.f), 14);

				++c;
			}
		}
	}
	/*
	renderer->setTexturingMode(1);
	textureManager->bind(_R("craftgame:noise4x4"));
	renderer->setColor(glm::vec4(1));
	renderer->drawRect(0, height - 256, 256, 256);*/
	// Joystick
	if (CGE::instance->joystick == 1) {
		{
			static int x = 0, y = 0, width = 0, height = 0;
			static float opacity = 0;
			static float pulse = 0;
			Gui* fc = getFocusedGui();
			if (fc) {
				opacity += millis * 3;

				int vx = fc->x, vy = fc->y;
				Gui* t = fc;
				while (t->parent) {
					t = t->parent;
					vx += t->x;
					vy += t->y;
				}

				x += roundf((vx - x) * glm::clamp(millis * 5, 0.5f, 1.f));
				y += roundf((vy - y) * glm::clamp(millis * 5, 0.5f, 1.f));
				width += roundf((fc->width - width) * glm::clamp(millis * 5, 0.5f, 1.f));
				height += roundf((fc->height - height) * glm::clamp(millis * 5, 0.5f, 1.f));
				pulse += millis * 0.8f;
				pulse = fmodf(pulse, 1.f);
			}
			else {
				opacity -= millis * 2;
			}
			opacity = glm::clamp(opacity, 0.f, 1.f);
			float p = pulse * 2;
			if (p > 1) {
				p = 2.f - p;
			}
			p = p * 0.4f + 0.6f;
			renderer->setColor(glm::vec4(1.f, 1.f, 1.f, opacity * p));
			renderer->setTexturingMode(0);
			renderer->drawBorder(x - 4, y - 4, width + 8, height + 8, 2);
		}

		size_t y = CGE::instance->height - 5;
		renderer->setTexturingMode(1);
		FontStyle fs;
		fs.size = 16;
		fs.color = glm::vec4(0.8f);
		for (std::map<Joystick::Keys, Joystick::JoystickButton>::iterator i = Joystick::_jb.begin(); i != Joystick::_jb.end(); i++) {
			static int _prev = 0;
			if (Joystick::getKeyState(i->first)) {
				if (!(_prev & i->first)) {
					if (i->second.call)
						i->second.call();
					_prev |= i->first;
				}
			}
			else {
				_prev &= ~i->first;
			}
			if (i->first == Joystick::START || i->first == Joystick::BACK || i->second.strength == 0) {
				continue;
			}
			
			static TextureAtlas atlas(CGE::instance->textureManager->loadTexture(_R("craftgame:res/icon/joy.png"), false), 8, 2);
			static CFont* f = CGE::instance->fontRegistry->getCFont(_R("craftgame:default"));

			y -= 36;

			atlas.bindTexture(Joystick::iconOf(i->first));
			renderer->setColor(glm::vec4(1.0));
			CGE::instance->renderer->drawTextureAtlas(CGE::instance->width - 32 - 5, y, 32, 32);
			CGE::instance->renderer->drawString(CGE::instance->width - 32 - 5 - 6, y + 6, i->second.name, Align::RIGHT, fs);
		}
		Joystick::_jb.clear();
	}
#ifdef _DEBUG
	{
		FontStyle fs;
		fs.color = glm::vec4(1, 0, 0, 1);
		renderer->drawString(width / 2, 2, "CRAFTGAME DEBUG BUILD", Align::CENTER, fs);
	}
#endif

	renderer->setTexturingMode(1);
	renderer->setColor(glm::vec4(1, 1, 1, 1));
	mainFB->bind(width, height);
#ifdef WINDOWS
	if (cursor) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cursort);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		CGE::instance->maskFB->begin();
		CGE::instance->maskShader->start();
		CGE::instance->maskShader->loadInt("texturing", 1);
		CGE::instance->maskShader->loadVector("c", glm::vec4(1));
		OS::getCursorPos(mouseX, mouseY);
		CGE::instance->renderer->drawRect(mouseX, mouseY, 32, 32);
		CGE::instance->maskShader->loadInt("texturing", 0);
		CGE::instance->maskFB->end();
		CGE::instance->standartGuiShader->start();
		CGE::instance->renderer->setMaskingMode(1);
		CGE::instance->renderer->blur(mouseX, mouseY, 32, 32);
		CGE::instance->standartGuiShader->start();
		glBindTexture(GL_TEXTURE_2D, cursort);
		CGE::instance->renderer->setMaskingMode(0);
		CGE::instance->renderer->setColor(glm::vec4(1, 1, 1, 0.6f));
		CGE::instance->renderer->setTexturingMode(1);
		CGE::instance->renderer->drawRect(mouseX, mouseY, 32, 32);
		glDisable(GL_BLEND);
	}
#endif
	glDisable(GL_BLEND);
	mainFB->unbind();
	guiShader->loadMatrix(transform, glm::mat4(1));
	guiShader->loadVector(uv, glm::vec2(0, 0));
	guiShader->loadVector(uv2, glm::vec2(1, 1));
	glBindTexture(GL_TEXTURE_2D, mainFB->texture);
	renderer->renderModel(Models::viewport);
	cursor = true;
	guiShader->stop();
	/*
	GLenum error;
	if (loaded && !opengl_error && (error = glGetError()) != GL_NO_ERROR)
	{
		opengl_error = true;
		CGE::instance->displayGuiScreen(new GuiScreenMessageDialog("OpenGL error", std::string("[") + std::to_string(opengl_error) + "] " + (char*)glewGetErrorString(error), [&](int)
		{
			opengl_error = false;
		}));
	}
	*/
	fps++;
	renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - renderTime;
	static int* pMaxFramerate = settings->getPropertyPointer<int>(_R("craftgame:graphics/maxFramerate"));
	int maxFramerate = *pMaxFramerate;
	if (!focus)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	if ((maxFramerate > 0 && maxFramerate < 13)) {
		int timeForFrame = 1000 / (maxFramerate * 10);
		int rt = renderTime.count();
		if (rt < timeForFrame) {
			std::this_thread::sleep_for(std::chrono::milliseconds(timeForFrame - rt));
		}
	}
	checkOpenGLError("End");
	setDebugString("Draw calls", std::to_string(_gDrawCalls));
	_gDrawCalls = 0;
}
void CGE::GameEngine::postRender(std::function<void()> _f)
{
	_postRender.push(_f);
}
void CGE::GameEngine::onClick(int btn) {
	for (size_t i = 0; i < guiWindows.size(); i++) {
		if (guiWindows[i]->visibility != Visibility::GONE) {
			glm::vec4 v = guiWindows[i]->getTransform() * glm::vec4(1, 1, 1, 1);

			int x = int((v.x - 1) / 2.f * CGE::instance->width);
			int y = int((v.y - 1) / -2.f * CGE::instance->height);
			if (x <= mouseX && x + guiWindows[i]->width >= mouseX && y <= mouseY && y + guiWindows[i]->height >= mouseY) {
				uiThread.push(std::function<void()>([&, x, y, i, btn]() {
					fcs = guiWindows[i];
					fcs->onMouseClick(mouseX - x, mouseY - y, btn);
				}));
				return;
			}
		}
	}
	uiThread.push(std::function<void()>([&, btn](){

		if (contextMenu) {
			contextMenu->onMouseClick(CGE::instance->mouseX, CGE::instance->mouseY, btn);
			contextMenu->hide();
		}
		if (guiScreens.empty())
		{
			if (CGE::instance->thePlayer && CGE::instance->thePlayer->worldObj) {
				switch (btn)
				{
				case 0:
					PlayerController::leftClick();
					break;
				case 1:
					PlayerController::rightClick();
					break;
				}
			}
		} else {
			fcs = guiScreens.back().get();
			fcs->onMouseClick(mouseX, mouseY, btn);
		}
	}));
}
void CGE::GameEngine::displayGuiScreen(GuiScreen* g) {
	uiThread.push(std::function<void()>([&, g]() {
		if (guiScreens.empty() && server) {
			ts<IServer>::r l(server);
			if (server)
			{
				l->sendPacket(new PForce(glm::vec3(0.f)));
			}
		}
		guiScreens.push_back(boost::shared_ptr<GuiScreen>(g));
		fcs = g;
		g->onLayoutChange(0, 0, width, height);
		if (!g->widgets.empty()) {
			g->focus = g->widgets.front().get();
		}
	}));
}
void CGE::GameEngine::displayGuiScreen(boost::shared_ptr<GuiScreen> g) {
	uiThread.push(std::function<void()>([&, g]() {
		guiScreens.push_back(g);
		fcs = g.get();
		g->onLayoutChange(0, 0, width, height);
		if (!g->widgets.empty()) {
			g->focus = g->widgets.front().get();
		}
	}));
}
void CGE::GameEngine::displayGuiWindow(GuiWindow * g)
{
	uiThread.push(std::function<void()>([&, g]() {
		guiWindows.push_back(g);
		g->onLayoutChange(g->x, g->y, g->width, g->height);
		fcs = g;
	}));
}
void CGE::GameEngine::removeGuiScreen(GuiScreen* s) {
	uiThread.push(std::function<void()>([&, s]() {
		for (std::deque<boost::shared_ptr<GuiScreen>>::iterator i = guiScreens.begin(); i != guiScreens.end(); i++) {
			if (i->get() == s) {
				if (s == fcs)
					fcs = nullptr;
				guiScreens.erase(i);
				break;
			}
		}
		if (guiScreens.empty() && CGE::instance->thePlayer)
		{
			OS::setCursorPos(width / 2, height / 2);
		}
	}));
}
void CGE::GameEngine::removeGuiWindow(GuiWindow * s)
{
	uiThread.push(std::function<void()>([&, s]() {
		for (std::vector<GuiWindow*>::iterator i = guiWindows.begin(); i != guiWindows.end(); i++) {
			if (*i == s) {
				if (s == fcs)
					fcs = nullptr;
				delete s;
				guiWindows.erase(i);
				break;
			}
		}
	}));
}

void CGE::GameEngine::rejectInputEvent(std::function<void(Keyboard::Key)> p)
{
	inputEvent = p;
}

extern float __swingAnim;
void debug_message(const std::string& m)
{
	INFO(std::string("Debug function: "));
	CGE::instance->chat->addChat(std::string("§e[") + _("debug.debug") + "] " + m);
}
void CGE::GameEngine::onKeyDown(Keyboard::Key key, byte data) {
	uiThread.push(std::function<void()>([&, key, data]() {
		if (inputEvent)
		{
			inputEvent(key);
			inputEvent = 0;
			return;
		}
		if (contextMenu)
			contextMenu->hide();
		
		if (server) {

			if (guiScreens.empty()) {
				for (size_t i = 0; i < kbRegistry->getData().size(); i++)
				{
					KeyBinding* kv = kbRegistry->getData()[i];
					if (Keyboard::isKeyDown(kv->getCurrentValue()))
					{
						kv->getProc()();
						return;
					}
				}
				if (key == Keyboard::Escape) {
					CGE::instance->displayGuiScreen(new GuiScreenPause);
					return;
				}

				switch (key)
				{
				case Keyboard::Space:
					if (data)
						_jump();
					break;
				case Keyboard::Num1:
					thePlayer->setItemIndex(0);
					break;
				case Keyboard::Num2:
					thePlayer->setItemIndex(1);
					break;
				case Keyboard::Num3:
					thePlayer->setItemIndex(2);
					break;
				case Keyboard::Num4:
					thePlayer->setItemIndex(3);
					break;
				case Keyboard::Num5:
					thePlayer->setItemIndex(4);
					break;
				case Keyboard::Num6:
					thePlayer->setItemIndex(5);
					break;
				case Keyboard::Num7:
					thePlayer->setItemIndex(6);
					break;
				case Keyboard::Num8:
					thePlayer->setItemIndex(7);
					break;
				case Keyboard::Num9:
					thePlayer->setItemIndex(8);
					break;
				case Keyboard::LButton:
					if (__swingAnim == 0)
						__swingAnim = 1;
					break;
				}

				if (Keyboard::isKeyDown(Keyboard::F3))
				{
					switch (key)
					{
					case Keyboard::Y: // Шейдеры
						maskShader->compile();
						standartGuiShader->compile();
						starShader->compile();
						particleShader->compile();
						 particleShader2->compile();
						blurShader->compile();
						defaultStaticShader->compile();
						skeletonShader->compile();
						shadowSkeletonShader->compile();
						shadowDefaultStaticShader->compile();
						for (auto s : gbufferShader)
						{
							s->compile();
						}
						debug_message(_("debug.shaders"));
						break;
					}
				}
			} else
			{
				if (key == Keyboard::Escape) {
					if (!dynamic_cast<GuiScreenDialog*>(guiScreens.back().get())) {
						guiScreens.back()->close();
						return;
					}
				}
			}
		}
		if (!guiScreens.empty() && guiScreens.back().get() != fcs)
			guiScreens.back()->onKeyDown(key, data);
		if (fcs)
			fcs->onKeyDown(key, data);
		if (data & 1) {
			if (Keyboard::isKeyDown(Keyboard::Key::F3) && CGE::instance->thePlayer && CGE::instance->thePlayer->worldObj) {
				switch (key) {
				case Keyboard::Key::R:
					for (Chunk*& c : CGE::instance->thePlayer->worldObj->renderList)
					{
						c->clear();
						c->markAsDirty();
					}
					break;
				}
			}
			switch (key) {
			case Keyboard::Key::F3:
				/*
#ifdef CGE_DEBUGGER
				if (Keyboard::isKeyDown(Keyboard::Key::LControl)) {
					for (size_t i = 0; i < guiWindows.size(); i++) {
						if (guiWindows[i]->getClassname() == "debug")
							return;
					}
					displayGuiWindow(new GuiWindowDebug);
				}
#endif
				*/
				break;
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

				std::time_t t = std::time(nullptr);
				std::tm tm;
				tm = *localtime(&t);
				char timebuf[64];
				std::strftime(timebuf, sizeof(timebuf), "%F_%H.%M.%S", &tm);
				ss << timebuf << ".png";
				imageLoader->savePng(&image, "../screenshots/" + ss.str());
				delete[] buffer;
				//logger->info("Screenshot saved as " + ss.str());
				chat->addChat(currentLocale->localize("gui.screenshot", spair("name", vr(ss.str())), spair("count", vr(int(d.list().size())))));
				break;
			}
		}
	}));
}
void CGE::GameEngine::onWheel(short d) {
	uiThread.push(std::function<void()>([&, d]() {
		if (fcs)
			fcs->onWheel(mouseX, mouseY, d);
		if (guiScreens.empty()) {
			if (CGE::instance->thePlayer && CGE::instance->thePlayer->worldObj) {
				short k = -d;
				while (k < 0)
				{
					k += 9;
				}
				k += CGE::instance->thePlayer->getItemIndex();
				k %= 9;
				CGE::instance->thePlayer->setItemIndex(k);
			}
		}
	}));
}
void CGE::GameEngine::onText(char c) {
	uiThread.push(std::function<void()>([&, c]() {
		if (fcs)
			fcs->onText(c);
	}));
}

void CGE::GameEngine::onJoystick()
{
	Joystick::fetchInfo();
	if (CGE::instance->thePlayer && CGE::instance->thePlayer->worldObj) {
		if (Joystick::getKeyState(Joystick::L1)) {
			uiThread.push([]()
			{
				PlayerController::leftClick();
			});
		}
		if (Joystick::getKeyState(Joystick::R1)) {
			uiThread.push([]()
			{
				PlayerController::rightClick();
			});
		}
	}
}

#include "GuiScreenLoad.h"
#include "PacketStartLogin.h"
#include "AnimationDialog.h"
#include "P01Ping.h"
void CGE::GameEngine::disconnect(std::string reason, std::string title) {
	threadPool.runAsync([&, reason, title]()
	{
		ts<IServer>::rw lock(server);
		if (server.isValid()) {
			disconnect();
			uiThread.push([&, reason, title]()
			{
				RemoteServer::playerList->clear();
				displayGuiScreen(new GuiScreenMessageDialog(_(title), reason, [](int) {}, MD_OK));
			});
		}
	});

}
void CGE::GameEngine::disconnect()
{
	CGE::instance->threadPool.runAsync([&] ()
	{
		ts<IServer>::rw lock(server);
		ADT::Node::globalAdt.clear();
		if (server.isValid()) {
			lock->close();
			server.destruct();
			delete thePlayer;
			thePlayer = nullptr;
		}
	});
	uiThread.push([&]()
	{
		for (size_t i = 0; i < guiScreens.size(); i++)
		{
			guiScreens[i]->close();
		}
		displayGuiScreen(new GuiScreenMainMenu);
	});
}
/**
 * \brief Подключается к серверу
 * \param url адрес, может быть доменом
 * \param port порт, 24565
 * \return Ничего
 */
void CGE::GameEngine::connectToServer(std::string url, unsigned short port) {
	ServerConnect::connectToServer(url, port);
}

void CGE::GameEngine::onSecondContext() {
	std::unique_lock<std::mutex> lcd(stm);
	stcv.wait(lcd);
	while (!secondThread.empty()) {
		secondThread.front()();
		secondThread.pop();
	}

}
void CGE::GameEngine::runOnSecondThread(std::function<void()> _func)
{
	secondThread.push(_func);
	stcv.notify_all();
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
		ss << "[" << __prev << "] => [" << s << "] :: [" << err << "]";
		char* d = (char*)gluGetString(err);
		if (d)
			ss << ": " << d;
		CGE::instance->chat->addChat(std::string("§e") + ss.str());
		CGE::instance->logger->err(ss.str());
	}
	if (PyErr_Occurred()) {
		std::stringstream ss;
		ss << "Python error has occurred [" << __prev << "] => [" << s << "]";
		CGE::instance->logger->err(ss.str());
		CGE::instance->chat->addChat(std::string("§e") + ss.str());
		PyErr_Print();
	}
	__prev = s;
}
Gui* CGE::GameEngine::getFocusedGui() {
	if (guiScreens.empty())
		return nullptr;
	GuiContainer* searchingIn = guiScreens.back().get();
	while (searchingIn->focus) {
		if (GuiContainer* c = dynamic_cast<GuiContainer*>(searchingIn->focus)) {
			searchingIn = c;
		}
		else {
			return searchingIn->focus;
		}
	}
	return nullptr;
}
#endif
//=====================================================================================//
/*                          BOTH CLIENT AND SERVER METHODS                             */
//=====================================================================================//
size_t CGE::GameEngine::assignGlobalUniqueEntityId() {
	return entityId++;
}
void CGE::ev(PythonEvent* e, std::function<void()> process)
{
	if (EventBus::pre(e) && EventBus::ev(e))
		process();
	EventBus::post(e);
	delete e;
}
