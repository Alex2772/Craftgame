#include "GuiTooltip.h"
#include "PContainerAction.h"
#include "POpenInventory.h"
#ifndef SERVER
#include "GuiInventory.h"
#include "GameEngine.h"
#include "Utils.h"

void GuiInventory::prepareRender()
{
	CGE::instance->skeletonShader->start();
	CGE::instance->skeletonShader->loadVector("viewPos", glm::vec3(0, 0, 0));
	CGE::instance->skeletonShader->loadMatrix("view", glm::mat4(1.0));
	CGE::instance->skeletonShader->loadInt("lightingC", 1);
	CGE::instance->skeletonShader->loadInt("block", 1);
	CGE::instance->renderer->disablePerspective();
	CGE::instance->defaultStaticShader->start();
	CGE::instance->staticShader->loadVector("viewPos", glm::vec3(0, 0, 0));
	CGE::instance->staticShader->loadMatrix("view", glm::mat4(1.0));
	CGE::instance->staticShader->loadInt("lightingC", 1);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	CGE::instance->renderer->renderFlags &= ~GameRenderer::RF_SSAO;
	CGE::instance->renderer->disablePerspective();
}

void GuiInventory::afterRender()
{
	CGE::instance->skeletonShader->startNonG();
	CGE::instance->skeletonShader->loadInt("lightingC", 0);
	CGE::instance->renderer->enablePerspective();
	CGE::instance->defaultStaticShader->startNonG();
	CGE::instance->renderer->enablePerspective();
	CGE::instance->defaultStaticShader->loadInt("lightingC", 0);
	CGE::instance->renderer->setTransform(glm::mat4(1.0));
	CGE::instance->guiShader->start();
	glDisable(GL_DEPTH_TEST);
	CGE::instance->renderer->renderFlags |= GameRenderer::RF_SSAO;
}

void GuiInventory::renderStack(int x, int y, ItemStack* stack, glm::mat4 t)
{
	int p = CGE::instance->renderer->renderType;
	CGE::instance->renderer->renderType = 0;
	//glm::vec3 vv = glm::vec3(-1.f, ((1 - float(y) / GameRenderer::viewportH) * 2.f - 1) - 80.f / GameRenderer::viewportH * 2.f, 0);
	
	y += 46;
	x += 8;
	if (!dynamic_cast<Block*>(stack->getItem())) {
		if (!(stack->getItem()->getTF() & Item::T_INVENTORY))
		{
			y -= 2;
			x += 17;
		}
		glm::vec3 vv = glm::vec3(float((x - CGE::instance->renderer->viewportW / 2) * 2) / CGE::instance->renderer->viewportW, float(-(y - CGE::instance->renderer->viewportH / 2) * 2) / CGE::instance->renderer->viewportH, 0);

		glm::mat4 c = glm::scale(glm::translate(glm::mat4(1.0), vv), glm::vec3(100.f / float(CGE::instance->renderer->viewportW), 100.f / float(CGE::instance->renderer->viewportH), 0.2f));
		if (stack->getItem()->getTF() & Item::T_INVENTORY) {
			c = glm::rotate(c, float(glm::radians(90.f / M_PI)), glm::vec3(0.5, 0, 0));
			c = glm::rotate(c, glm::radians(-45.f), glm::vec3(0, 0, 0.5f));
		} else
		{
			c = glm::scale(c, glm::vec3(0.6f));
			c = glm::rotate(c, glm::radians(180.f), glm::vec3(0, 1.f, 0));
		}
		CGE::instance->renderer->transform = c * t;
	} else
	{
		x += 18;
		y -= 6;
		glm::vec3 vv = glm::vec3(float((x - CGE::instance->renderer->viewportW / 2) * 2) / CGE::instance->renderer->viewportW, float(-(y - CGE::instance->renderer->viewportH / 2) * 2) / CGE::instance->renderer->viewportH, 0);

		glm::mat4 c = glm::scale(glm::translate(glm::mat4(1.0), vv), glm::vec3(60.f / float(CGE::instance->renderer->viewportW), 60.f / float(CGE::instance->renderer->viewportH), 0.2f));
		
		CGE::instance->renderer->transform = c * t;
	}
	stack->getItem()->getRenderer()->render(IItemRenderer::INVENTORY, stack->getItem(), stack);
	CGE::instance->renderer->renderType = p;
}

GuiInventory::GuiInventory(std::vector<InventoryContainer*> c)
{
	containers.push_back(CGE::instance->thePlayer->inventory);
	containers.insert(containers.begin() + 1, c.begin(), c.end());
	CGE::instance->thePlayer->interface->players[CGE::instance->thePlayer] = CGE::instance->thePlayer->inventory;
	
	bg.parent = this;
	bg.bottom_left = GuiExtrusion::LEFT;
	bg.bottom = GuiExtrusion::CENTER;
	bg.bottom_right = GuiExtrusion::RIGHT;
	tooltip = new GuiTooltip(std::string());
}

GuiInventory::~GuiInventory()
{
	ts<IServer>::r lock(CGE::instance->server);
	if (CGE::instance->thePlayer && CGE::instance->thePlayer->interface) {
		CGE::instance->thePlayer->interface->removePlayer(CGE::instance->thePlayer);
		if (CGE::instance->thePlayer->tmpInv)
		{
			delete CGE::instance->thePlayer->tmpInv;
			CGE::instance->thePlayer->tmpInv = nullptr;
		}
		lock->sendPacket(new POpenInventory);
	}
}

void GuiInventory::renderWidgets(int rf, int mouseX, int mouseY)
{
	ts<IServer>::r lock(CGE::instance->server);
	if (!CGE::instance->thePlayer->interface)
	{
		close();
		return;
	}
	GuiIngameScreen::renderWidgets(rf, mouseX, mouseY);
	static CFont* f = CGE::instance->fontRegistry->getCFont("inventory");
	mouseX -= x;
	mouseY -= y;
	size_t idCnt = 0;
	CGE::instance->renderer->drawString((width - 52 * 9) / 2, height - 52 * 4 - 46, _("game.inventory"), Align::LEFT, TextStyle::SIMPLE, glm::vec4(0.5f, 0.5f, 0.5f, 1.f), 17, f);

	GuiTooltip* t = dynamic_cast<GuiTooltip*>(tooltip);
	t->setTitle(std::string());
	t->setText(std::string());
	for (InventoryContainer*& inventory : containers) {
		for (Slot*& s : inventory->slots)
		{
			static glm::vec4 c_bg = Utils::fromHex(0x8b8b8bff);
			static glm::vec4 c_light = glm::vec4(1);
			static glm::vec4 c_dark = Utils::fromHex(0x373737ff);


			CGE::instance->renderer->setTexturingMode(0);

			// Background
			CGE::instance->renderer->setColor(c_bg);
			CGE::instance->renderer->drawRect(s->x, s->y, s->width, s->height);

			// Dark
			CGE::instance->renderer->setColor(c_dark);
			CGE::instance->renderer->drawRect(s->x, s->y, 50, 2);
			CGE::instance->renderer->drawRect(s->x, s->y + 2, 2, 48);

			// Light
			CGE::instance->renderer->setColor(c_light);
			CGE::instance->renderer->drawRect(s->x + s->width - 2, s->y + 2, 2, 48);
			CGE::instance->renderer->drawRect(s->x + 2, s->y + s->height - 2, 50, 2);

			if (!CGE::instance->thePlayer->tmpInv) {
				if (mouseX >= s->x && mouseX <= s->x + s->width && mouseY >= s->y && mouseY <= s->y + s->height)
				{
					s->highlight += CGE::instance->millis * 8.f;
					if (s->stack) {
						t->setTitle(s->stack->getName());
					}
				}
				else
				{
					s->highlight -= CGE::instance->millis * 3.f;
				}
				s->highlight = glm::clamp(s->highlight, 0.f, 1.f);

				if (s->highlight > 0.f)
				{
					CGE::instance->renderer->setTexturingMode(0);
					CGE::instance->renderer->setColor(glm::vec4(1.f, 1.f, 1.f, .5f * s->highlight));
					CGE::instance->renderer->drawRect(s->x + 2, s->y + 2, s->width - 4, s->height - 4);
				}
			}

			CGE::instance->renderer->drawString(s->x + 2, s->y + 2, std::to_string(idCnt++), Align::LEFT, TextStyle::SIMPLE, glm::vec4(0.4f, 0.4f, 0.4f, 1.f), 12);

		}
	}
	prepareRender();
	for (InventoryContainer*& inventory : containers) {
		for (Slot*& s : inventory->slots)
		{
			if (s->stack && s->stack->getItem()->getRenderer())
			{
				renderStack(s->x + GuiInventory::x, s->y + GuiInventory::y, s->stack);
			}
		}
	}
	afterRender();
	calculateMask();

	for (InventoryContainer*& inventory : containers) {
		for (Slot*& s : inventory->slots)
		{
			if (s->stack && s->stack->getCount() > 1)
			{
				CGE::instance->renderer->drawString(s->x + 47, s->y + 49 - 16, std::to_string(s->stack->getCount()), Align::RIGHT, TextStyle::SHADOW, glm::vec4(1.f), 14, f);
			}
			if (CGE::instance->thePlayer->tmpInv) {
				if (mouseX >= s->x && mouseX <= s->x + s->width && mouseY >= s->y && mouseY <= s->y + s->height)
				{
					s->highlight += CGE::instance->millis * 8.f;
				}
				else
				{
					s->highlight -= CGE::instance->millis * 3.f;
				}
				s->highlight = glm::clamp(s->highlight, 0.f, 1.f);

				if (s->highlight > 0.f)
				{
					CGE::instance->renderer->setTexturingMode(0);
					CGE::instance->renderer->setColor(glm::vec4(1.f, 1.f, 1.f, .5f * s->highlight));
					CGE::instance->renderer->drawRect(s->x + 2, s->y + 2, s->width - 4, s->height - 4);
				}
			}
		}
	}
	prepareRender();
	if (CGE::instance->thePlayer->tmpInv)
	{
		CGE::instance->cursor = false;
		glClear(GL_DEPTH_BUFFER_BIT);
		renderStack(mouseX + x - 26, mouseY - 26 + y, CGE::instance->thePlayer->tmpInv);
	}
	afterRender();
	calculateMask();
	if (CGE::instance->thePlayer->tmpInv)
	{
		if (CGE::instance->thePlayer->tmpInv->getCount() > 1)
		{
			CGE::instance->renderer->setMaskingMode(0);
			glm::mat4 m = CGE::instance->renderer->transform;
			CGE::instance->renderer->transform = glm::mat4(1.0);
			CGE::instance->renderer->drawString(CGE::instance->mouseX - 26 + 47, CGE::instance->mouseY - 26 + 49 - 16, std::to_string(CGE::instance->thePlayer->tmpInv->getCount()), Align::RIGHT, TextStyle::SHADOW, glm::vec4(1.f), 14, f);
			CGE::instance->renderer->setMaskingMode(1);
			CGE::instance->renderer->transform = m;
		}
	}
	if (tooltip && dynamic_cast<GuiTooltip*>(tooltip)) {
		if (!dynamic_cast<GuiTooltip*>(tooltip)->empty()) {
			CGE::instance->postRender([&, rf]() {
				tooltip->visibility = VISIBLE;
				tooltip->x = CGE::instance->mouseX + 18;
				tooltip->y = CGE::instance->mouseY + 32;
				if (tooltip->x + tooltip->width > CGE::instance->width)
				{
					tooltip->x = CGE::instance->mouseX - 15 - tooltip->width;
				}
				if (tooltip->y + tooltip->height > CGE::instance->height)
				{
					tooltip->y = CGE::instance->mouseY - 18 - tooltip->height;
				}
				tooltip->render(rf ^ RenderFlags::MOUSE_HOVER, CGE::instance->mouseX, CGE::instance->mouseY);
			});
		}
		else {
			tooltip->visibility = GONE;
		}
	}
}

void GuiInventory::render(int flags, int mouseX, int mouseY)
{
	GuiIngameScreen::render(flags, mouseX, mouseY);
	glm::mat4 t = getTransform();
	glm::vec4 c = getColor();
	drawWidget(&bg, t, c, flags, mouseX, mouseY);
}

void GuiInventory::onLayoutChange(int x, int y, int w, int h)
{
	GuiIngameScreen::onLayoutChange((w - 492) / 2, h - 400, 492, 400);
	bg.onLayoutChange(0, 0, GuiInventory::width, GuiInventory::height);
}
inline void packet_send(uint16_t count, size_t from, size_t to)
{
	PContainerAction* p = new PContainerAction;
	p->count = count;
	p->from = from;
	p->to = to;
	ts<IServer>::r(CGE::instance->server)->sendPacket(p);
}
void GuiInventory::onMouseClick(int mouseX, int mouseY, int button)
{
	GuiIngameScreen::onMouseClick(mouseX, mouseY, button);
	mouseX -= x;
	mouseY -= y;
	size_t index = 1;
	switch (button)
	{
	case 0:
		for (InventoryContainer*& inventory : containers) {
			for (size_t i = 0; i < inventory->slots.size(); i++)
			{
				Slot*& s = inventory->slots[i];
				if (mouseX >= s->x && mouseX <= s->x + s->width && mouseY >= s->y && mouseY <= s->y + s->height)
				{
					if (CGE::instance->thePlayer->tmpInv) {
						if (s->accepts(CGE::instance->thePlayer->tmpInv)) {
							if (s->stack && s->stack->getItem() != CGE::instance->thePlayer->tmpInv->getItem()) {
								packet_send(-1, 0, index);
								CGE::instance->thePlayer->tmpInv = inventory->swap(i, CGE::instance->thePlayer->tmpInv);
							}
							else {
								packet_send(CGE::instance->thePlayer->tmpInv->getCount(), 0, index);
								inventory->put(i, CGE::instance->thePlayer->tmpInv->getCount(), CGE::instance->thePlayer->tmpInv);
							}
						}
					}
					else if (s->stack) {
						packet_send(s->stack->getCount(), index, 0);
						inventory->get(i, s->stack->getCount(), CGE::instance->thePlayer->tmpInv);
					}
					return;
				}
				index++;
			}
		}
		break;
	case 1:
		for (InventoryContainer*& inventory : containers) {
			for (size_t i = 0; i < inventory->slots.size(); i++)
			{
				Slot*& s = inventory->slots[i];
				if (mouseX >= s->x && mouseX <= s->x + s->width && mouseY >= s->y && mouseY <= s->y + s->height)
				{
					if (CGE::instance->thePlayer->tmpInv)
					{
						if (s->stack && s->stack->getItem() != CGE::instance->thePlayer->tmpInv->getItem()) {
							packet_send(-1, 0, index);
							CGE::instance->thePlayer->tmpInv = inventory->swap(i, CGE::instance->thePlayer->tmpInv);
						}
						else {
							packet_send(1, 0, index);
							inventory->put(i, 1, CGE::instance->thePlayer->tmpInv);
						}
					}
					else if (s->stack)
					{
						uint16_t c = (s->stack->getCount() + 2 - 1) / 2;
						packet_send(c, index, 0);
						inventory->get(i, c, CGE::instance->thePlayer->tmpInv);
					}
					return;
				}
				index++;
			}
		}
		break;
	default:
		break;
	}
}
#endif
