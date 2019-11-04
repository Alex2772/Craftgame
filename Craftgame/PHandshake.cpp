#include "PHandshake.h"
#include "GameEngine.h"
#include "P02JoinWorld.h"
#include "NetServerHandler.h"
#include "RemoteServer.h"
#include "GuiScreenMessageDialog.h"

void PHandshake::write(ByteBuffer& buffer)
{
	buffer << info << id;
	ADT::serialize(buffer, adt);
}

void PHandshake::read(ByteBuffer& buffer)
{
	buffer >> info >> id;
	adt = ADT::deserialize(buffer)->toMap();
}
#ifndef SERVER
extern GuiScreenMessageDialog* _connectionDialog;
#endif
void PHandshake::onReceived()
{
#ifndef SERVER
	ts<IServer>::rw lock(CGE::instance->server);
	RemoteServer* r = dynamic_cast<RemoteServer*>(lock.get());
	EntityPlayerSP* pl = new EntityPlayerSP(r->world, id, *CGE::instance->gameProfile);
	pl->adtTag = adt;
	r->world->spawnEntity(CGE::instance->thePlayer = pl, DPos(0, 0, 0));
	lock->sendPacket(new P02JoinWorld());
	CGE::instance->uiThread.push([&]() {
		if (_connectionDialog)
			_connectionDialog->setText(_("gui.connecting.connecting.loading"));
	});
#endif
}

void PHandshake::onReceived(EntityPlayerMP* s)
{
}
