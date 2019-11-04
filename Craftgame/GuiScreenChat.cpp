#include "PAutocomplete.h"
#ifndef SERVER
#include "GuiScreenChat.h"


GuiScreenChat::GuiScreenChat()
{

	addWidget(f = new CustomTextField());
	f->displayBackground = false;
	CGE::instance->chat->full = true;
	CGE::instance->chat->scroll = 0;

	animations.push_back(animation = new AnimationFade(this));
	animation->onAnimationEnd = [&](State)
	{
		focus = f;
	};
	if (CGE::instance->thePlayer)
		CGE::instance->thePlayer->updateState(CGE::instance->thePlayer->state | (1 << 1));

	cmds = CGE::instance->commandRegistry->getCommandList();
	std::sort(cmds.begin(), cmds.end(), [](Command* l, Command* r)
	{
		return l->getName() > r->getName();
	});
}

GuiScreenChat::~GuiScreenChat()
{
	if (chatHistoryIndex == 0) {
		CGE::instance->chat->clientQueue.push_front(f->getText());
	} else
	{
		CGE::instance->chat->clientQueue[chatHistoryIndex - 1] = f->getText();
	}
	while (CGE::instance->chat->clientQueue.size() > 50)
	{
		CGE::instance->chat->clientQueue.pop_back();
	}
	CGE::instance->chat->full = false;
	if (CGE::instance->thePlayer)
		CGE::instance->thePlayer->updateState(CGE::instance->thePlayer->state & ~(1 << 1));
}

void GuiScreenChat::render(int flags, int mouseX, int mouseY)
{
	GuiScreen::render(flags, mouseX, mouseY);
	//	CGE::instance->renderer->blur(2, height - 30, width - 4, 30);
	CGE::instance->renderer->setColor(glm::vec4(0, 0, 0, 0.5f) * getColor());
	CGE::instance->renderer->setTexturingMode(0);
	CGE::instance->renderer->drawRect(2, height - 30, width - 4, 30);

	if (!f->getText().empty() && f->getText()[0] == '/' && f->getText().size() > 1)
	{
		std::string t = f->getText().c_str() + 1;
		std::vector<std::string> splt = Parsing::splitString(t, " ");
		std::string& fst = splt[0];
		size_t it = 0;
		for (size_t i = 0; i < cmds.size(); i++)
		{
			static CFont* font = CGE::instance->fontRegistry->getCFont("default");
			if (splt.size() > 0 && cmds[i]->getName() == fst)
			{
				static size_t prevSpaces = 0;
				size_t spaces = 0;
				for (size_t i = 1; i < f->cursor; i++)
				{
					if (t[i] == ' ')
						spaces++;
				}
				if (prevSpaces != spaces)
				{
					autocomplete.clear();
				}
				prevSpaces = spaces;
				std::vector<std::string> usage = Parsing::splitString(std::string("/") + cmds[i]->constructCommandUsage(), " ");
				int y = height - 30 * (2) - 5;

				CGE::instance->renderer->setColor(glm::vec4(0, 0, 0, 0.5f) * getColor());
				CGE::instance->renderer->setTexturingMode(0);
				CGE::instance->renderer->drawRect(2, y, width - 4, 29);
				CGE::instance->renderer->setColor(getColor());
				FontStyle fs;
				size_t counter = 0;
				size_t x = 6;
				for (std::vector<std::string>::iterator j = usage.begin(); j != usage.end(); j++)
				{
					fs.color = (counter == spaces || ((j + 1 == usage.end()) && spaces >= usage.size() && cmds[i]->getArgTypes()[cmds[i]->getArgTypes().size() - 1].type == T_STRING)) ? glm::vec4(1.f) : glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
					CGE::instance->renderer->drawString(x, y + 5, *j, Align::LEFT, fs);
					x += font->length(*j) + font->length(" ");
					counter++;
				}
				break;
			} else if (t.length() <= cmds[i]->getName().length() && Parsing::startsWith(cmds[i]->getName(), t)) {
				int y = height - 30 * (it + 2) - 5;
				if (y + 30 < 0)
					break;
				CGE::instance->renderer->setColor(glm::vec4(0, 0, 0, 0.5f) * getColor());
				CGE::instance->renderer->setTexturingMode(0);
				CGE::instance->renderer->drawRect(2, y, width - 4, 29);
				CGE::instance->renderer->setColor(getColor());
				FontStyle fs;
				size_t k = 0;
				for (; k < t.length(); k++)
				{
					if (t[k] != cmds[i]->getName()[k])
					{
						break;
					}
				}
				std::string f = std::string("/") + cmds[i]->getName().substr(0, k);

				CGE::instance->renderer->drawString(6, y + 5, f, Align::LEFT, fs);
				fs.color = glm::vec4(0.5f, 0.5f, 0.5f, 1.f);
				CGE::instance->renderer->drawString(6 + font->length(f), y + 5, cmds[i]->getName().substr(k), Align::LEFT, fs);
				it++;
			}
		}
	}
}

void GuiScreenChat::onKeyDown(Keyboard::Key key, byte data)
{
	if (key != Keyboard::Tab)
	{
		autocompleteMode = false;
	}
	switch (key)
	{
	case Keyboard::Tab:
	{
		if (autocompleteMode) {
			autocompleteNext();
			f->first = true;
		} else {
			std::vector<std::string> splt = Parsing::splitString(f->getText(), " ");
			if (Parsing::endsWith(f->getText(), " "))
				splt.push_back("");
			if (splt.size() > 1) {
				size_t cur = 0;
				index = 0;
				for (size_t i = 0; i < splt.size(); i++)
				{
					cur += splt[i].length() + 1;
					if (f->cursor < cur)
					{
						index = i - 1;
					}
				}
				f->first = true;
				ts<IServer>::rw(CGE::instance->server)->sendPacket(new PAutocomplete(f->getText(), index));
			}
			else {
				size_t count = 0;
				Command* cm = nullptr;
				if (!f->getText().empty() && f->getText()[0] == '/')
				{
					std::string t = f->getText().c_str() + 1;
					size_t it = 0;
					for (size_t i = 0; i < cmds.size(); i++)
					{
						if (t.length() <= cmds[i]->getName().length() && Parsing::startsWith(cmds[i]->getName(), t)) {
							cm = cmds[i];
							count++;
						}
					}
				}
				if (count == 1)
				{
					std::string ft = "/" + cm->getName();
					if (!cm->getArgTypes().empty())
						ft += " ";
					f->setText(ft);
					f->selection = -1;
					f->first = true;
				}
			}
		}
	}
		break;
	case Keyboard::Key::Return: {
		std::string s = f->getText();
		if (!s.empty())
		{
			CGE::instance->chat->sendMessage(s);
		}
	}
	case Keyboard::Key::Escape:
		close();
		break;
	case Keyboard::Key::Up:
		if (!CGE::instance->chat->clientQueue.empty()) {
			if (chatHistoryIndex == 0)
			{
				storedString = f->getText();
			}
			chatHistoryIndex++;
			if (CGE::instance->chat->clientQueue.size() <= (chatHistoryIndex - 1))
			{
				chatHistoryIndex--;
			}
			f->setText(CGE::instance->chat->clientQueue[chatHistoryIndex - 1]);
		}
		break;
	case Keyboard::Key::Down:
		if (chatHistoryIndex > 0)
		{
			chatHistoryIndex--;
			if (chatHistoryIndex == 0)
			{
				f->setText(storedString);
			} else
			{
				f->setText(CGE::instance->chat->clientQueue[chatHistoryIndex - 1]);
			}

		}
		break;
	default:
		GuiScreen::onKeyDown(key, data);
	}
}

void GuiScreenChat::autocompleteNext()
{
	if (!autocomplete.empty()) {
		autocompleteIndex++;
		autocompleteIndex %= autocomplete.size();
		std::vector<std::string> list = Parsing::splitString(f->getText(), " ");
		while (list.size() <= index + 1)
			list.push_back("");
		list[index + 1] = autocomplete[autocompleteIndex];
		std::string fn;
		for (size_t i = 0; i < list.size(); i++)
		{
			if (i != 0)
				fn += " ";
			fn += list[i];
		}
		int cursor = f->cursor + fn.length() - f->getText().length();
		f->setText(fn);
		f->selection = -1;
		f->cursor = cursor;
	}
}

void GuiScreenChat::autocompleteReceived(const std::vector<std::string>& a)
{
	autocompleteMode = true;
	autocomplete = a;
	autocompleteIndex = 0;
	if (autocomplete.size() == 1)
	{
		autocompleteNext();
	}
}
#endif
