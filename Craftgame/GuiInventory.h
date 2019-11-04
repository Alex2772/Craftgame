#pragma once
#ifndef SERVER
#include "GuiExtrusion.h"
#include "GuiIngameScreen.h"
#include "InventoryContainer.h"

class GuiInventory: public GuiIngameScreen
{
private:
	std::vector<InventoryContainer*> containers;
	GuiExtrusion bg;
public:
	static void prepareRender();
	static void afterRender();
	static void renderStack(int x, int y, ItemStack* stack, glm::mat4 transform = glm::mat4(1.0));
	GuiInventory(std::vector<InventoryContainer*> c);
	virtual ~GuiInventory();
	virtual void renderWidgets(int rf, int mouseX, int mouseY) override;
	virtual void render(int flags, int mouseX, int mouseY) override;
	virtual void onLayoutChange(int x, int y, int w, int h) override;
	virtual void onMouseClick(int mouseX, int mouseY, int button) override;
};
#endif