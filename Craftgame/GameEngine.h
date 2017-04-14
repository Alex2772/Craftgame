#pragma once
#include "global.h"
#include "Logger.h"
#ifndef SERVER
#include "Shader.h"
#include "BlurShader.h"
#include "VAO.h"
#include "GameRenderer.h"
#include "ImageLoader.h"
#include "TextureManager.h"
#include "FontRegistry.h"
#include "FrameBuffer.h"
#include "SecFrameBuffer.h"
#include "MaskFrameBuffer.h"
#include "GuiScreen.h"
#include "ModelRegistry.h"
#include "MaterialRegistry.h"
#include "LightSource.h"
#include "SkinManager.h"
#include "Keyboard.h"
#include "Camera.h"
#include <deque>
#include "GameProfile.h"
#include "SoundManager.h"
class FontRegistry;
class GuiConnectingPanel;
extern "C" {
	#include <ft2build.h>
	#include <freetype\freetype.h>
}
#endif
#ifdef WINDOWS
#include <Windows.h>
#endif
#include "Config.h"
#include "ThreadPool.h"
#include "AsyncQueue.h"
#include "Socket.h"
#include "PacketRegistry.h"
#include "CommandRegistry.h"

class Server;

namespace CGE {
	class GameEngine {
	private:
#ifndef SERVER
		GuiConnectingPanel* connectingPanel;
		void sizeSpecific();
		int mouseX;
		int mouseY;
#endif
		size_t entityId = 0;
	public:
		/* Fields */
		ThreadPool threadPool;
#ifndef SERVER
		int width;
		int height;
		GameRenderer* renderer;
		VAO* vao;
		ImageLoader* imageLoader;
		TextureManager* textureManager;
		Framebuffer* mainFB;
		SecFramebuffer* secFB;
		SecFramebuffer* thirdFB;
		MaskFramebuffer* maskFB;
		bool glewInitiated = false;
		SkinManager skinManager;
		Camera* camera;
		std::vector<LightSource*> light;
		bool focus = true;
		FT_Library ft;
		std::deque<GuiScreen*> guiScreens;
		GameProfile* gameProfile = nullptr;
		SoundManager* soundManager;
#endif
		Logger* logger;
		Server* server = nullptr;
		Config::ConfigFile* settings = nullptr;
		AsyncQueue < std::function<void()>> uiThread;
		AsyncQueue < std::function<void()>> secondThread;
		float millis = 0;
		Socket* socket = nullptr;
		PacketRegistry* packetRegistry;
		CommandRegistry* commandRegistry;
#ifndef SERVER
		/* Matrix */
		glm::mat4 projection;
		glm::mat4 projection2;
		glm::mat4 view;

		/* Registries */
		FontRegistry* fontRegistry;
		MaterialRegistry* materialRegistry;
		ModelRegistry* modelRegistry;

		/* Shaders */
		Shader* guiShader = nullptr;
		Shader* maskShader;
		Shader* standartGuiShader;
		BlurShader* blurShader;
		Shader* staticShader;
#endif
		/* Functions */
		GameEngine();
		~GameEngine();
		void preInit();
		size_t assignGlobalUniqueEntityId();
#ifndef SERVER
		void postInit();
		void render(int mouseX, int mouseY);
		void onResize(int width, int height);
		void displayGuiScreen(GuiScreen* g);
		void onClick(int button);
		void onKeyDown(Keyboard::Key key, byte data);
		void onWheel(short d);
		void onText(char c);
		void onSecondContext();
		unsigned char displayDialog(std::string title, std::string text);
		void disconnect(Socket* s, std::string reason, std::string title = "Disconnected");
		void removeGuiScreen(GuiScreen* target);
#endif
		void connectToServer(std::string url, unsigned short port);
	};
	void checkOpenGLError(std::string s);
	extern GameEngine* instance;
}
#include "Server.h"

void CGE_setVSync(bool sync);

#define INFO(a) CGE::instance->logger->info(a)
#define WARN(a) CGE::instance->logger->warn(a)

#ifdef _DEBUG
#define __DEBUG(a) CGE::instance->logger->debug(std::to_string(a))
#else
#define __DEBUG(a)
#endif

#define __CGE_CE(a) CGE::checkOpenGLError(a)
