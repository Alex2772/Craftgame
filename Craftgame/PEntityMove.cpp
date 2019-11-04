#include "PEntityMove.h"
#include "GameEngine.h"

void PEntityMove::onReceived(EntityPlayerMP* s)
{
	/*
	 * DEPRECATED
	if (s->getId() == id)
	{
		AABB b = s->createAABB();
		glm::vec3 tr = d.toVec3() - s->getPos().toVec3();
		b.move(tr);
		if (glm::length(tr) < 1.4f && !s->worldObj->checkCollision(b))
		{
			s->setPosNonServer(d);
			ts<IServer>::rw(CGE::instance->server)->sendPacketAllExcept(new PEntityMove(s, false), s->getNetHandler());
		}
		else
		{
			WARN(std::string("Player ") + s->getGameProfile().getUsername() + " moves wrongly!");
			s->getNetHandler()->sendPacket(new PEntityMove(s, false));
		}
		CGE_EVENT("move", CGE_P("entity", static_cast<Entity*>(s)), [&]()
		{

		});
	}*/
	//CGE::instance->getCEServer()->dropPlayer(s->getId());
}
#ifndef SERVER
#include "GuiScreenMessageDialog.h"
extern GuiScreenMessageDialog* _connectionDialog;
#endif
void PEntityMove::onReceived()
{
#ifndef SERVER
	if (Entity* e = CGE::instance->thePlayer->worldObj->getEntity(id)) {
		if (df)
			e->setPos(d);
		else
			e->setPosNonServer(d);
		if (EntitySkeleton* sk = dynamic_cast<EntitySkeleton*>(e))
		{
			sk->updateAnimation("move", 1, true);
		}
		if (CGE::instance->thePlayer == e)
		{
			if (_connectionDialog)
				CGE::instance->uiThread.push([]()
				{
					if (_connectionDialog) {
						_connectionDialog->close();
						_connectionDialog = nullptr;
					}
				});
		}
	}
#endif
}
