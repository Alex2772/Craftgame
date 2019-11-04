#include "EntityPlayerSP.h"
#include "GameEngine.h"
#include "PFlyMode.h"
#include "PChangeSlot.h"
extern float _3rdp_aa;
void EntityPlayerSP::render()
{
#ifndef SERVER
	if ((CGE::instance->renderer->renderType & GameRenderer::RENDER_SHADOW) || _3rdp_aa > 0.5f) {
		EntityPlayerMP::render();
	} else {
		EntitySkeleton::render();
	}
	CGE::instance->setDebugString("isFlying", std::to_string(isFlying()));
#endif
}
void EntityPlayerSP::setFlying(bool b)
{
	ts<IServer>::r(CGE::instance->server)->sendPacket(new PFlyMode(b));
}

EntityPlayerSP::EntityPlayerSP(World* w, const size_t _id, const GameProfile& _g)
	: EntityPlayerMP(w, _id, _g)
{

}

void EntityPlayerSP::setLook(float x, float y)
{
	EntityPlayerMP::setLook(x, y);
#ifndef SERVER
	CGE::instance->camera->yaw = x;
	CGE::instance->camera->pitch = y;
#endif
}

void EntityPlayerSP::tick()
{
	EntityPlayerMP::tick();
}


void EntityPlayerSP::setItemIndex(byte index)
{
	PChangeSlot* p = new PChangeSlot();
	p->index = index;
	ts<IServer>::r(CGE::instance->server)->sendPacket(p);
	if (itemAnim >= 0.5f)
		prevItemIndex = getItemIndex();
	EntityPlayerMP::setItemIndex(index);
}

byte EntityPlayerSP::getPrevItemIndex()
{
	return prevItemIndex;
}

void EntityPlayerSP::itemAnimation()
{
	if (itemAnim > 0.5f)
	{
		itemAnim = 1.f - itemAnim;
	}
}
