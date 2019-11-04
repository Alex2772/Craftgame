#include "GuiScreenMessageDialog.h"
#include "GuiLabel.h"
#include "Utils.h"
#include "KeyBinding.h"
#include "colors.h"
#include "BinaryOutputStream.h"
#include "BinaryInputStream.h"
#include "GuiTextField.h"
#include "SGuiSeekBar.h"
#ifndef SERVER
#include "GuiScreenSettings.h"
#include "GameEngine.h"
#include "GuiList.h"
#include "GuiTabButton.h"
#include "GuiSeekBar.h"
#include <glm/gtc/matrix_transform.hpp>
#include "AnimationOpen.h"
#include "AnimationFade.h"
#include "GuiSwitch.h"
#include "GuiTooltip.h"
#include "GuiMenu.h"
#include "Dir.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "GuiProgressBar.h"
#include "ByteBuffer.h"
#include <marshal.h>


class GuiDialogPacker: public GuiScreenMessageDialog
{
private:
	Dir dir;
	std::thread* t;
	void compile(File& f)
	{
		volatile bool kek = false;
		std::string s;
		char* tmpbuf;
		size_t asize = 0;
		CGE::instance->uiThread.push([&, f]()
		{
			std::ifstream fis(f.path, std::ios::binary | std::ios::ate);
			size_t size = fis.tellg();
			fis.seekg(0, std::ios::beg);
			char* tmp = new char[size + 1];
			fis.read(tmp, size);
			fis.close();
			tmp[size] = '\0';

			PyObject *builtins = PyEval_GetBuiltins();
			PyObject *compile = PyDict_GetItemString(builtins, "compile");
			std::string name = f.name() + "c";
			PyObject* code = PyObject_CallFunction(compile, "sss", tmp, name.c_str(), "exec");
			PyObject* str = PyMarshal_WriteObjectToString(code, 2);
			Py_DECREF(code);

			char* buf;
			Py_ssize_t bsize;

			PyBytes_AsStringAndSize(str, &buf, &bsize);
			delete[] tmp;
			tmpbuf = new char[bsize];
			memcpy(tmpbuf, buf, bsize);
			Py_DECREF(str);
			asize = bsize;
			kek = true;

		});
		while (!kek);

		BinaryOutputStream fos(f.path + "c");
		fos.write(tmpbuf, asize);
		fos.close();
	}
	GuiProgressBar* progressBar = nullptr;
	void glue(ByteBuffer& buf, const Dir& dir)
	{
		std::vector<File> list = dir.list();
		buf << list.size();
		for (File& f : list)
		{
			if (f.isDir())
			{
				buf << byte(0x01);
				std::string name = f.name();
				buf << name;
				ByteBuffer tmp;
				glue(tmp, Dir(f.path));
				buf << tmp.size();
				buf.put(tmp.get(), tmp.size());
			} else if (f.isFile())
			{
				std::string filename = f.name();
				if (Parsing::endsWith(filename, ".py") || 
					filename == "package.json")
					continue;
				buf << byte(0x00);
				buf << filename;
				std::ifstream fis(f.path, std::ios::binary | std::ios::ate);
				size_t size = fis.tellg();
				fis.seekg(0, std::ios::beg);
				char* tmp = new char[size];
				fis.read(tmp, size);
				fis.close();
				buf << size;
				buf.put(tmp, size);
				delete[] tmp;
				CGE::instance->uiThread.push([&]()
				{
					progressBar->value += 1.f;
				});
			}
		}
	}
public:
	void setTextAsync(std::string t)
	{
		CGE::instance->uiThread.push([&, t]()
		{
			setText(t);
		});
	}

	GuiDialogPacker(Dir d):
		GuiScreenMessageDialog(_("gui.settings.pack"), "", [](int)
	{
		
	}, 0),
		dir(d)
	{
		setText(_("gui.preparing"));
		t = new std::thread([&]()
		{
			try {
				boost::property_tree::ptree pt;
				std::string _path = dir.path + "/package.json";
				std::ifstream fis(_path);
				if (fis.good()) {
					boost::property_tree::ptree pt;
					boost::property_tree::read_json(fis, pt);
					fis.close();

					std::string package_name = pt.get_child("name").get_value<std::string>();
					std::string version = pt.get_child("version").get_value<std::string>();
					BinaryOutputStream fos(package_name + "-" + version + ".cgx");
					fos.writeTemplate<byte>(0x3c);
					fos.writeString(package_name);
					fos.writeString(version);

					for (File& f : dir.list())
					{
						if (Parsing::endsWith(f.name(), ".py"))
						{
							setTextAsync(CGE::instance->currentLocale->localize("gui.settings.pack.pyc", spair("name", vr(f.name()))));
							compile(f);
						}
					}
					CGE::instance->uiThread.push([&]()
					{
						addWidget(progressBar = new GuiProgressBar);
						progressBar->onLayoutChange(4, height - 30, width - 8, 30);
						progressBar->si = "";
					});
					setTextAsync(_("gui.settings.pack.counting"));
					size_t count = dir.countRecursive();
					CGE::instance->uiThread.push([&, count]()
					{
						progressBar->max = count;
						progressBar->value = 0.f;
						setText(_("gui.settings.pack.gluing"));
					});
					ByteBuffer buf;
					glue(buf, dir);
					setTextAsync(_("gui.settings.pack.compressing"));
					ByteBuffer c;
					Utils::compress(buf, c);
					setTextAsync(_("gui.settings.pack.writing"));
					fos << c.size();
					fos.write(c.get(), c.size());
					fos << size_t(pt.get_child("mods").size());
					for (boost::property_tree::ptree::value_type& v : pt.get_child("mods"))
					{
						std::string modid = v.second.get_child("modid").get_value<std::string>();
						std::string name = v.second.get_child("name").get_value<std::string>();
						std::string version = v.second.get_child("version").get_value<std::string>();
						int cge = v.second.get_child("cge").get_value<int>();

						auto c = v.second.get_child_optional("side");
						std::string side = "both";
						if (c)
						{
							side = c->get_value<std::string>();
						}
						byte s = 0;
						if (side == "both")
							s = 0;
						else if (side == "client")
							s = 1;
						else if (side == "server")
							s = 2;
						fos.writeString(name);
						fos.writeString(modid);
						fos.writeString(version);
						fos.writeInt(cge);
						fos.writeTemplate<byte>(s);
					}

					fos.close();
					CGE::instance->uiThread.push([&]()
					{
						close();
					});
				} else
				{
					throw std::runtime_error(_("gui.settings.pack.error.nomanifest"));
				}
			} catch (std::exception e)
			{
				CGE::instance->uiThread.push([&, e]()
				{
					close();
					CGE::instance->displayGuiScreen(new GuiScreenMessageDialog(_("gui.settings.pack.error"), e.what()));
				});
			}
			catch (boost::python::error_already_set e)
			{
				std::string s = Utils::pythonErrorString();
				CGE::instance->uiThread.push([&, s]()
				{
					close();
					CGE::instance->displayGuiScreen(new GuiScreenMessageDialog(_("gui.settings.pack.error"), s));
				});
			}
		});
	}
	virtual ~GuiDialogPacker()
	{
		t->join();
		delete t;
	}
};

class Settings_Mod: public Gui
{
private:
	Mod* m;
	GuiScreenSettings* sett;
public:
	void refresh();
	Settings_Mod(Mod* _m, GuiScreenSettings* s):
		m(_m),
		sett(s)
	{
		height = 40;
		contextMenu = new GuiMenu;
		if (CGE::instance->settings->getProperty<bool>(_R("craftgame:dev"))) {
			contextMenu = new GuiMenu;
			contextMenu->addWidget(new GuiButton(_("gui.settings.mods.pack"), [&](int, int, int)
			{
				std::string& path = m->file;
				if (Parsing::endsWith(path, ".py"))
				{
					std::vector<std::string> s = Parsing::splitString(path, "/");
					if (s.size() == 3)
					{
						Dir dir("mods/" + s[1]);
						CGE::instance->displayGuiScreen(new GuiDialogPacker(dir));
					}
				}
			}));
		}
	}

	virtual void render(int flags, int mouseX, int mouseY) override {
		Gui::render(flags, mouseX, mouseY);
		CGE::instance->renderer->setTexturingMode(0);
		CGE::instance->renderer->setColor(glm::vec4(1.f, 1.f, 1.f, 0.2f));
		CGE::instance->renderer->drawRect(0, 0, width, height);
		FontStyle fs;
		fs.size = 18;
		CGE::instance->renderer->drawString(4, 4, m->name, Align::LEFT, fs);
		static CFont* f = CGE::instance->fontRegistry->getCFont(_R("default"));
		size_t offset = f->length(m->name, fs.size);
		fs.color = glm::vec4(1.f, 1.f, 1.f, 0.4f);
		fs.size = 14;
		CGE::instance->renderer->drawString(4 + offset + 4, 4, m->version, Align::LEFT, fs);
	}
};

class Settings_KeyBinding: public Gui
{
private:
	KeyBinding* kb;
	std::string name;
	float a = 0.f;
	bool capturing = false;
public:
	Settings_KeyBinding(KeyBinding* k, std::string n):
		kb(k),
		name(n)
	{
		
	}
	~Settings_KeyBinding()
	{
		CGE::instance->skipInputEventRejection();
	}
	virtual void render(int flags, int mouseX, int mouseY) override {
		Gui::render(flags, mouseX, mouseY);
		static size_t c = CGE::instance->guiShader->getUniform("c");
		glm::vec4 color = getColor();
		CGE::instance->guiShader->loadVector(c, Utils::fromHex(COLOR_BLUE) * color);
		CGE::instance->renderer->drawString(2, 9, _(name.c_str()), Align::LEFT, TextStyle::SIMPLE, color);
		
		{
			const int w = 100;
			const int h = height - 10;
			const int x = width - w - 10;
			const int y = 5;
			static size_t c = CGE::instance->guiShader->getUniform("c");
			static size_t texturing = CGE::instance->guiShader->getUniform("texturing");
			if (!(flags & RenderFlags::NO_BLUR)) {
				CGE::instance->renderer->blur(x, y, w, h);
			}
			CGE::instance->guiShader->loadInt(texturing, 0);
			if (capturing) {
				a += CGE::instance->millis;
				a = fmod(a, 2.f);
				float itog = ((cos(a / 3.14f * 10) + 1) / 2.f);
				glm::vec4 clr = glm::mix(glm::vec4(1), Utils::fromHex(0xc7ddf3aa), itog);
				//CGE::instance->renderer->drawShadow(x, y, w, h, glm::vec4(1.f, 1.f, 1.f, itog * 0.5f + 0.2f), 1, 3);
				//calculateMask();
				CGE::instance->guiShader->loadVector(c, clr * color);
			} else
			{
				CGE::instance->guiShader->loadVector(c, glm::vec4(0.4f, 0.4f, 0.4f, flags & RenderFlags::MOUSE_HOVER ? 0.3f : 0.1f) * color);
			}
			CGE::instance->renderer->drawRect(x, y, w, h);
			CGE::instance->guiShader->loadInt(texturing, 1);
			if (capturing) {
				CGE::instance->renderer->drawString(x + w / 2, y + 7, "...", Align::CENTER, TextStyle::SIMPLE, glm::vec4(0.1, 0.1, 0.1, 1) * color);
			} else
			{
				CGE::instance->renderer->drawString(x + w / 2, y + 7, Keyboard::getName(kb->getCurrentValue()), Align::CENTER, TextStyle::SIMPLE, (flags & RenderFlags::MOUSE_HOVER ? glm::vec4(1, 1, 1, 1) : glm::vec4(0.8, 0.8, 0.8, 0.9)) * color);
			}
			CGE::instance->guiShader->loadVector(c, glm::vec4(1, 1, 1, 1));
		}
	}
	virtual void onMouseClick(int mouseX, int mouseY, int button) override {
		capturing = true;
		CGE::instance->rejectInputEvent([&](Keyboard::Key k)
		{
			kb->setCurrentValue(k);
			CGE::instance->kbRegistry->save(CGE::instance->settings);
			capturing = false;
		});
	}
};

class GuiScreenFileDialog: public GuiScreenMessageDialog
{
private:
	std::function<void(std::string)> callback;
	GuiTextField* textField;
public:

	GuiScreenFileDialog(std::function<void(std::string)> f)
		: GuiScreenMessageDialog(_("gui.file"), "", [&](int b)
	{
		if (b == 1)
			callback(textField->getText());
	}, MD_OK | MD_CANCEL),
		callback(f)
	{
		addWidget(textField = new GuiTextField);
	}

	virtual void onLayoutChange(int x, int y, int width, int height) override {
		GuiScreenMessageDialog::onLayoutChange(x, y, width, height);
		textField->onLayoutChange(4, 55, GuiScreenMessageDialog::width - 8, 30);
	}
};


float f = -40;
GuiScreenSettings::GuiScreenSettings():
	GuiScreenDialog(new AnimationOpen(this))
{

	addWidget(new GuiButton(_("gui.done"), std::function<void(int, int, int)>([&](int, int, int) {
		close();
	})));
	GuiList* l = new GuiList;
	l->addWidget(new GuiTabButton(_("gui.settings.game"), std::function<void(int, int, int)>([&](int, int, int) {
		GuiScreenSettings::widgets[2]->animations.back()->state = State::play;
		GuiScreenSettings::widgets[3]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[2]->visibility = Visibility::VISIBLE;
		GuiScreenSettings::widgets[4]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[5]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[6]->animations.back()->state = State::inverse;
	})));
	l->addWidget(new GuiTabButton(_("gui.settings.graphics"), std::function<void(int, int, int)>([&](int, int, int) {
		GuiScreenSettings::widgets[2]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[3]->animations.back()->state = State::play;
		GuiScreenSettings::widgets[3]->visibility = Visibility::VISIBLE;
		GuiScreenSettings::widgets[4]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[5]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[6]->animations.back()->state = State::inverse;
	})));
	l->addWidget(new GuiTabButton(_("gui.settings.input"), std::function<void(int, int, int)>([&](int, int, int) {
		GuiScreenSettings::widgets[2]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[3]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[4]->visibility = Visibility::VISIBLE;
		GuiScreenSettings::widgets[4]->animations.back()->state = State::play;
		GuiScreenSettings::widgets[5]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[6]->animations.back()->state = State::inverse;
	})));
	l->addWidget(new GuiTabButton(_("gui.settings.misc"), std::function<void(int, int, int)>([&](int, int, int) {
		GuiScreenSettings::widgets[2]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[3]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[4]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[5]->animations.back()->state = State::play;
		GuiScreenSettings::widgets[5]->visibility = Visibility::VISIBLE;
		GuiScreenSettings::widgets[6]->animations.back()->state = State::inverse;
	})));
	l->addWidget(new GuiTabButton(_("gui.settings.mods"), std::function<void(int, int, int)>([&](int, int, int) {
		GuiScreenSettings::widgets[2]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[3]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[4]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[5]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[6]->animations.back()->state = State::play;
		GuiScreenSettings::widgets[6]->visibility = Visibility::VISIBLE;
	})));
	{
		if (CGE::instance->settings->getProperty<bool>(_R("craftgame:dev"))) {
			GuiMenu* m = new GuiMenu;
			m->addWidget(new GuiButton(_("gui.settings.mods.pack"), [&](int, int, int)
			{
				CGE::instance->displayGuiScreen(new GuiScreenFileDialog([](std::string path)
				{
					Dir dir(path);
					CGE::instance->displayGuiScreen(new GuiDialogPacker(dir));
				}));
			}));
			l->widgets[4]->contextMenu = m;
		}
	}
	l->focus = l->widgets[0].get();
	addWidget(l);

	// Game
	{
		GuiList* l = new GuiList;
		l->addWidget(new GuiTabButton(_("gui.settings.game"), std::function<void(int, int, int)>([&](int, int, int) {
		})));
		l->animations.push_back(new AnimationFade(l));
		l->animations.back()->state = State::stop;
		l->animations.back()->onAnimationEnd = std::function<void(State)>([&](State s) {
			GuiScreenSettings::widgets[2]->visibility = s == State::play ? Visibility::VISIBLE : Visibility::GONE;
		});
		addWidget(l);
	}

	// Graphics
	{
		GuiList* l = new GuiList;
		{
			Gui* j;
			l->addWidget(j = new GuiSwitch(_("gui.settings.graphics.nn"), std::function<void(bool)>([](bool v) {
				CGE::instance->settings->setProperty(_R("craftgame:graphics/normal"), v);
				CGE::instance->settings->apply();
			}), CGE::instance->settings->getProperty<bool>(_R("craftgame:graphics/normal"))));

			j->tooltip = new GuiTooltip(_("gui.settings.graphics.nn"), _("gui.settings.graphics.nn.tt"));
		}
		/*
		l->addWidget(new GuiSwitch(_("gui.settings.graphics.animations"), std::function<void(bool)>([](bool v) {
			CGE::instance->settings->setProperty(_R("craftgame:graphics/animations"), v);
			CGE::instance->settings->apply();
		}), CGE::instance->settings->getProperty<bool>(_R("craftgame:graphics/animations"))));*/
		{
			Gui* j = new GuiSwitch(_("gui.settings.graphics.uiblur"), std::function<void(bool)>([](bool v) {
				CGE::instance->settings->setProperty(_R("craftgame:gui/blur"), v);
				CGE::instance->settings->apply();
			}), CGE::instance->settings->getProperty<bool>(_R("craftgame:gui/blur")));
			j->tooltip = new GuiTooltip(_("gui.settings.graphics.uiblur"), _("gui.settings.graphics.uiblur.tt"));
			l->addWidget(j);
		}
		/*
		{

			SGuiSeekBar* s = new SGuiSeekBar(_("gui.settings.graphics.lighting"), "graphics/lighting", [](int v) {

				//CGE::instance->settings->setProperty(_R("craftgame:graphics/skybox"), v);

				switch (v)
				{
				case 2:
					return _("gui.settings.max");
				case 1:
					return _("gui.settings.simplified");
				}
				return _("gui.settings.low");
			}, [](int v) {

				for (Shader* k : CGE::instance->gbufferShader) {
					k->defines["GRAPHICS"] = v;
					k->compile();
				}

			}, 2, 0);

			l->addWidget(s);
		}*/
		{

			SGuiSeekBar* s = new SGuiSeekBar(_("gui.settings.graphics.sky"), "graphics/sky", [](int v) {

				//CGE::instance->settings->setProperty(_R("craftgame:graphics/skybox"), v);
				if (v == 1) {
					return _("gui.settings.max");
				}
				return _("gui.settings.low");
			},
				[](int v)
			{
				CGE::instance->uiThread.push([v]() {
					CGE::instance->gbufferShader[1]->compile();
				});
			}, 1);

			l->addWidget(s);
		}
		{

			SGuiSeekBar* s = new SGuiSeekBar(_("gui.settings.graphics.ao"), "graphics/ao", [](int v) {
				switch (v)
				{
				case 0:
					return _("gui.settings.disabled");
				case 1:
					return _("gui.settings.low");
				case 2:
					return _("gui.settings.max");
				default:
					return _("gui.settings.extreme");
				}
				
			},
				[](int v)
			{
				CGE::instance->uiThread.push([]() {
					CGE::instance->gbufferShader[1]->compile();
				});
			}, 3);

			l->addWidget(s);
		}
		{

			SGuiSeekBar* s = new SGuiSeekBar(_("gui.settings.graphics.reflections"), "graphics/reflections", [](int v) {
				switch (v)
				{
				case 0:
					return _("gui.settings.disabled");
				case 1:
					return _("gui.settings.low");
				case 2:
					return _("gui.settings.max");
				}
			},
				[](int v)
			{
				CGE::instance->uiThread.push([v]() {
					CGE::instance->gbufferShader[1]->compile();
				});
			}, 2);

			l->addWidget(s);
		}
		l->animations.push_back(new AnimationFade(l));
		l->animations.back()->state = State::stop;
		l->animations.back()->onAnimationEnd = std::function<void(State)>([&](State s) {
			GuiScreenSettings::widgets[3]->visibility = s == State::play ? Visibility::VISIBLE : Visibility::GONE;
		});
		l->visibility = Visibility::GONE;
		addWidget(l);

	}
	// Input
	{
		GuiList* l = new GuiList; 
		for (size_t i = 0; i < CGE::instance->kbRegistry->getGroupData().size(); i++)
		{
			std::string s = std::string("craftgame:input/") + CGE::instance->kbRegistry->getGroupData()[i]->name;
			GuiLabel* ls = new GuiLabel(_(s));
			ls->setTextSize(18);
			l->addWidget(ls);
			s += "/";
			for (size_t j = 0; j < CGE::instance->kbRegistry->getGroupData()[i]->data.size(); j++)
			{
				KeyBinding* kb = CGE::instance->kbRegistry->getGroupData()[i]->data[j];
				std::string r = s + kb->name;
				r = _(r);
				l->addWidget(new Settings_KeyBinding(kb, r));
			}
		}
		l->animations.push_back(new AnimationFade(l));
		l->animations.back()->state = State::stop;
		l->animations.back()->onAnimationEnd = std::function<void(State)>([&](State s) {
			GuiScreenSettings::widgets[4]->visibility = (s == State::play ? Visibility::VISIBLE : Visibility::GONE);
		});
		l->visibility = Visibility::GONE;
		addWidget(l);
	}

	// Misc
	{
		GuiList* l = new GuiList;
		l->addWidget(new GuiSwitch(_("gui.settings.misc.debug"), std::function<void(bool)>([](bool v) {
			CGE::instance->settings->setProperty(_R("craftgame:debug"), v);
			CGE::instance->settings->apply();
		}), CGE::instance->settings->getProperty<bool>(_R("craftgame:debug"))));
		l->addWidget(new GuiSwitch(_("gui.settings.misc.net_debug"), std::function<void(bool)>([](bool v) {
			CGE::instance->settings->setProperty(_R("craftgame:net_debug"), v);
			CGE::instance->settings->apply();
		}), CGE::instance->settings->getProperty<bool>(_R("craftgame:net_debug"))));
		l->addWidget(new GuiSwitch(_("gui.settings.misc.dev"), std::function<void(bool)>([](bool v) {
			CGE::instance->settings->setProperty(_R("craftgame:dev"), v);
			CGE::instance->settings->apply();
		}), CGE::instance->settings->getProperty<bool>(_R("craftgame:dev"))));
		{
			GuiSeekBar* s = new GuiSeekBar(std::function<std::string(int)>([](int v) {
				
				CGE::instance->settings->setProperty(_R("craftgame:graphics/maxFramerate"), v);
				if (v == 0) {
					CGE::instance->uiThread.push(std::function<void()>([]() {
						CGE_setVSync(true);
					}));
					return _P("gui.settings.misc.fr", spair("value", _("gui.settings.misc.fr.vsync")));
				}
				else if (v == 13) {
					CGE::instance->uiThread.push(std::function<void()>([]() {
						CGE_setVSync(false);
					}));
					return _P("gui.settings.misc.fr", spair("value", _("gui.none")));
				}
				else {
					CGE::instance->uiThread.push(std::function<void()>([]() {
						CGE_setVSync(false);
					}));
					return _P("gui.settings.misc.fr", spair("value", 10 * v));
				}
			}), 13, CGE::instance->settings->getProperty<int>(_R("craftgame:graphics/maxFramerate")));
			l->addWidget(s);
			s->onValueChangingStop = std::function<void(int)>([](int) {
				CGE::instance->settings->apply();
			});
		}
		l->animations.push_back(new AnimationFade(l));
		l->animations.back()->state = State::stop;
		l->animations.back()->onAnimationEnd = std::function<void(State)>([&](State s) {
			GuiScreenSettings::widgets[5]->visibility = s == State::play ? Visibility::VISIBLE : Visibility::GONE;
		});
		l->visibility = Visibility::GONE;
		addWidget(l);
	}
	// Mods
	{
		mods = new GuiList;


		mods->animations.push_back(new AnimationFade(l));
		mods->animations.back()->state = State::stop;
		mods->animations.back()->onAnimationEnd = std::function<void(State)>([&](State s) {
			GuiScreenSettings::widgets[6]->visibility = s == State::play ? Visibility::VISIBLE : Visibility::GONE;
		});
		mods->visibility = Visibility::GONE;
		refreshMods();
		addWidget(mods);
	}
	l->widgets[0]->onMouseClick(0, 0, 0);
}
void GuiScreenSettings::render(int rf, int mouseX, int mouseY) {
	GuiScreenDialog::render(rf, mouseX, mouseY);
	glm::vec4 color = getColor();
	CGE::instance->renderer->drawString(width / 2, 16, _("gui.settings"), Align::CENTER, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.6) * color, 22);
}

void GuiScreenSettings::onKeyDown(Keyboard::Key key, byte data) {
	GuiScreenDialog::onKeyDown(key, data);
	if (key == Keyboard::Key::Escape) {
		close();
	}
}
void GuiScreenSettings::onLayoutChange(int x, int y, int width, int height) {
	GuiScreenDialog::onLayoutChange(width * 0.05, height * 0.1, width * 0.9, height * 0.8);
	int w = width * 0.3f;
	if (w > 350)
		w = 350;
	widgets[0]->onLayoutChange(0, 0 + Gui::height - 40, w, 40);
	widgets[1]->onLayoutChange(0, 60, w, Gui::height - 100);
	for (size_t i = 0; i < 5; i++)
		widgets[2 + i]->onLayoutChange(w + 2, 60, Gui::width - w - 2, Gui::height - 60);
	widgets[2]->parent = this;
}

void GuiScreenSettings::refreshMods()
{
	mods->widgets.clear();
	for (Mod* m : CGE::instance->modManager->mods)
	{
		mods->addWidget(new Settings_Mod(m, this));
	}
}

#endif
