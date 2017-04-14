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

float f = -40;
GuiScreenSettings::GuiScreenSettings():
	GuiScreenDialog(new AnimationOpen(this))
{

	addWidget(new GuiButton("Done", std::function<void(int, int, int)>([&](int, int, int) {
		close();
	})));
	GuiList* l = new GuiList;
	l->addWidget(new GuiTabButton("Game", std::function<void(int, int, int)>([&](int, int, int) {
		GuiScreenSettings::widgets[2]->animations.back()->state = State::play;
		GuiScreenSettings::widgets[3]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[2]->visibility = Visibility::VISIBLE;
		GuiScreenSettings::widgets[4]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[5]->animations.back()->state = State::inverse;
	})));
	l->addWidget(new GuiTabButton("Graphics", std::function<void(int, int, int)>([&](int, int, int) {
		GuiScreenSettings::widgets[2]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[3]->animations.back()->state = State::play;
		GuiScreenSettings::widgets[3]->visibility = Visibility::VISIBLE;
		GuiScreenSettings::widgets[4]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[5]->animations.back()->state = State::inverse;
	})));
	l->addWidget(new GuiTabButton("Input", std::function<void(int, int, int)>([&](int, int, int) {
		GuiScreenSettings::widgets[2]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[3]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[4]->visibility = Visibility::VISIBLE;
		GuiScreenSettings::widgets[4]->animations.back()->state = State::play;
		GuiScreenSettings::widgets[5]->animations.back()->state = State::inverse;
	})));
	l->addWidget(new GuiTabButton("Misc", std::function<void(int, int, int)>([&](int, int, int) {
		GuiScreenSettings::widgets[2]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[3]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[4]->animations.back()->state = State::inverse;
		GuiScreenSettings::widgets[5]->animations.back()->state = State::play;
		GuiScreenSettings::widgets[5]->visibility = Visibility::VISIBLE;
	})));
	
	l->focus = l->widgets[0];
	addWidget(l);

	// Game
	{
		GuiList* l = new GuiList;
		l->addWidget(new GuiTabButton("Game", std::function<void(int, int, int)>([&](int, int, int) {
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
		l->addWidget(new GuiSwitch("Normal mapping", std::function<void(bool)>([](bool v) {
			CGE::instance->settings->setProperty(_R("craftgame:graphics/normal"), v);
			CGE::instance->settings->apply();
		}), CGE::instance->settings->getProperty<bool>(_R("craftgame:graphics/normal"))));
		l->addWidget(new GuiSwitch("Animations", std::function<void(bool)>([](bool v) {
			CGE::instance->settings->setProperty(_R("craftgame:graphics/animations"), v);
			CGE::instance->settings->apply();
			CGE::instance->threadPool.runAsync([]() {
				CGE::instance->displayDialog("Restart", "Restart the game to apply the changes.");
			});
		}), CGE::instance->settings->getProperty<bool>(_R("craftgame:graphics/animations"))));
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
		l->addWidget(new GuiTabButton("123", std::function<void(int, int, int)>([&](int, int, int) {

		})));
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
		l->addWidget(new GuiSwitch("Debug", std::function<void(bool)>([](bool v) {
			CGE::instance->settings->setProperty(_R("craftgame:debug"), v);
			CGE::instance->settings->apply();
		}), CGE::instance->settings->getProperty<bool>(_R("craftgame:debug"))));
		{
			GuiSeekBar* s = new GuiSeekBar(std::function<std::string(int)>([](int v) {
				
				CGE::instance->settings->setProperty(_R("craftgame:graphics/maxFramerate"), v);
				if (v == 0) {
					CGE::instance->uiThread.push(std::function<void()>([]() {
						CGE_setVSync(true);
					}));
					return std::string("Max frame rate: VSync");
				}
				else if (v == 13) {
					CGE::instance->uiThread.push(std::function<void()>([]() {
						CGE_setVSync(false);
					}));
					return std::string("Max frame rate: None");
				}
				else {
					CGE::instance->uiThread.push(std::function<void()>([]() {
						CGE_setVSync(false);
					}));
					return std::string("Max frame rate: ") + std::to_string(10 * v);
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
	l->widgets[0]->onMouseClick(0, 0, 0);
}
void GuiScreenSettings::render(int rf, int mouseX, int mouseY) {
	GuiScreenDialog::render(rf, mouseX, mouseY);
	glm::vec4 color = getColor();
	CGE::instance->renderer->drawString(16, 16, "Settings", Align::LEFT, TextStyle::SIMPLE, glm::vec4(1, 1, 1, 0.6) * color, 22);
}

void GuiScreenSettings::onKeyDown(Keyboard::Key key, byte data) {
	GuiScreenDialog::onKeyDown(key, data);
	if (key == Keyboard::Key::ESCAPE) {
		close();
	}
}
void GuiScreenSettings::onLayoutChange(int x, int y, int width, int height) {
	GuiScreenDialog::onLayoutChange(x, y, width, height);
	int w = width * 0.3f;
	if (w > 350)
		w = 350;
	widgets[0]->onLayoutChange(0, 0 + Gui::height - 40, w, 40);
	widgets[1]->onLayoutChange(0, 60, w, Gui::height - 100);
	for (size_t i = 0; i < 4; i++)
		widgets[2 + i]->onLayoutChange(w + 2, 60, Gui::width - w - 2, Gui::height - 60);
	widgets[2]->parent = this;
}
#endif