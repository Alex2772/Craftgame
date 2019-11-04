#pragma once
#include "global.h"
#include "Logger.h"
#include "Mod.h"
#include "IServer.h"
#ifndef SERVER
#include "KeyBindingRegistry.h"
#include "Shader.h"
#include "BlurShader.h"
#include "VAOHelper.h"
#include "GameRenderer.h"
#include "ImageLoader.h"
#include "TextureManager.h"
#include "FontRegistry.h"
#include "Framebuffer.h"
#include "SecFramebuffer.h"
#include "MaskFramebuffer.h"
#include "GBuffer.h"
#include "GuiScreen.h"
#include "LightSource.h"
#include "SkinManager.h"
#include "Keyboard.h"
#include "Camera.h"
#include <deque>
#include "GameProfile.h"
#include "SoundManager.h"
#include "GuiWindow.h"
#include "Debugger.h"
#include <condition_variable>
#include "GuiMenu.h"
class CFontRegistry;
extern "C" {
	#include <ft2build.h>
	#include <freetype/freetype.h>
}
#endif
#ifdef WINDOWS
#include <Windows.h>
#endif
#include "Config.h"
#include "ThreadPool.h"
#include "AsyncQueue.h"
#include "Socket.h"
#include "GORegistry.h"
#include "PacketRegistry.h"
#include "CommandRegistry.h"
#include "EntityRegistry.h"
#include "ModelRegistry.h"
#include "MaterialRegistry.h"
#include "i18n.h"
#include "StaticShader.h"
#include "Chat.h"
#include "ModManager.h"
#include "EntityPlayerSP.h"
#include "Depthbuffer.h"

struct DebugInfo
{
	std::string key;
	std::string value;
	float highlightAnimation = 1.f;
};
namespace CGE {
	class GameEngine {
	private:
		std::vector<std::thread*> threads;
#ifndef SERVER
		void sizeSpecific();
		std::mutex stm;
		AsyncQueue < std::function<void()>> secondThread;
		AsyncQueue<std::function<void()>> _postRender;
		std::function<void(Keyboard::Key)> inputEvent;
		std::vector<DebugInfo> debugInfo;
#endif
		size_t entityId = 0;
	public:
		/* Fields */
		ThreadPool threadPool;
		Locale* currentLocale;
		Chat* chat;
#ifndef SERVER
		TextureAtlas taParticles;
		byte joystick = 0;
		EntityPlayerSP* thePlayer = nullptr;
		int mouseX;
		int mouseY;
		std::condition_variable stcv;
		int width;
		int height;
		GameRenderer* renderer;
		VAOHelper* vao;
		ImageLoader* imageLoader;
		TextureManager* textureManager;
		Framebuffer* mainFB;
		SecFramebuffer* secFB;
		SecFramebuffer* thirdFB;
		MaskFramebuffer* maskFB;
		Depthbuffer* shadowFB;
		GBuffer* gBuffer;
		bool glewInitiated = false;
		SkinManager skinManager;
		Camera* camera;
		ts<std::vector<LightSource*>> light;
		bool focus = true;
		FT_Library ft;
		std::deque<boost::shared_ptr<GuiScreen>> guiScreens;
		GameProfile* gameProfile = nullptr;
		SoundManager* soundManager;
		std::vector<GuiWindow*> guiWindows;
		GuiMenu* contextMenu = nullptr;
		Gui* fcs = nullptr;
		bool cursor = true;
		KeyBindingRegistry* kbRegistry;
		std::map<std::string, boost::shared_ptr<Shader>> customShaders;
#endif
		ts<IServer> server;
		ModManager* modManager;
		void createThread(std::function<void()> f);
		Logger* logger;
		Config::ConfigFile* settings = nullptr;
		AsyncQueue < std::function<void()>> uiThread;
		float millis = 0;
		Socket* socket = nullptr;
		GORegistry* goRegistry;
		PacketRegistry* packetRegistry;
		CommandRegistry* commandRegistry;
		EntityRegistry* entityRegistry;
		MaterialRegistry* materialRegistry;
		ModelRegistry* modelRegistry;
#ifndef SERVER
		/* Matrix */
		glm::mat4 projection;
		glm::mat4 projection2;
		glm::mat4 view;

		/* Registries */
		CFontRegistry* fontRegistry;

		/* Shaders */
		Shader* guiShader = nullptr;
		Shader* maskShader;
		Shader* standartGuiShader;
		Shader* starShader;
		Shader* particleShader;
		Shader* particleShader2;
		Shader* currentShader = nullptr;
		std::vector<Shader*> gbufferShader;
		BlurShader* blurShader;
		StaticShader* defaultStaticShader;
		StaticShader* skeletonShader;
		StaticShader* shadowSkeletonShader;
		StaticShader* shadowDefaultStaticShader;
		StaticShader* staticShader = nullptr;
#endif
		/* Functions */
		GameEngine();
		~GameEngine();
		void preInit();
		size_t assignGlobalUniqueEntityId();
		void postInit();
#ifndef SERVER
		void focusGain();
		void focusLost();
		void init();
		void render(int mouseX, int mouseY);
		void postRender(std::function<void()> _f);
		void onResize(int width, int height);
		void displayGuiScreen(GuiScreen* g);
		void displayGuiScreen(boost::shared_ptr<GuiScreen> g);
		void displayGuiWindow(GuiWindow* g);
		void onClick(int button);
		void onKeyDown(Keyboard::Key key, byte data);
		void onWheel(short d);
		void onText(char c);
		void onJoystick();
		void onSecondContext();
		void runOnSecondThread(std::function<void()> _func);
		void disconnect();
		void disconnect(std::string reason, std::string title = "gui.disconnected.disconnected");
		void removeGuiScreen(GuiScreen* target);
		void removeGuiWindow(GuiWindow* target);
		void rejectInputEvent(std::function<void(Keyboard::Key)> p);
		void skipInputEventRejection();
		void setDebugString(std::string key, std::string value);
		Gui* getFocusedGui();
#endif
		void connectToServer(std::string url, unsigned short port);
	};
	void checkOpenGLError(std::string s);
	void ev(PythonEvent* e, std::function<void()> process);
	extern GameEngine* instance;
}
void CGE_setVSync(bool sync);

#define INFO(a) CGE::instance->logger->info(a)
#define WARN(a) CGE::instance->logger->warn(a)

#ifndef _
#define _(x) CGE::instance->currentLocale->localize(x)
#define _P(x, ...) CGE::instance->currentLocale->localize(x, __VA_ARGS__)
#endif
#define CGE_EVENT_CHECK if (CGE::instance->modManager->initiated)
#define CGE_EVENT(name, ar, func) CGE::ev((new PythonEvent(_R(name)))ar, func)
#define CGE_P(name, e) ->param(name, e)
