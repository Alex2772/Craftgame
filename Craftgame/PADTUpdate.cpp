#include "PADTUpdate.h"
#include "GameEngine.h"

PADTUpdate::PADTUpdate(boost::shared_ptr<ADT::Node> f):
	PADTUpdate()
{
	n = f;
}

void PADTUpdate::write(ByteBuffer& buffer)
{
	ADT::serialize(buffer, n);
	buffer << n->id;
}

void PADTUpdate::read(ByteBuffer& buffer)
{
	n = ADT::deserialize(buffer, false);
	buffer >> n->id;
}

void PADTUpdate::onReceived()
{
	std::map<size_t, boost::shared_ptr<ADT::Node>>::iterator i = ADT::Node::globalAdt.find(n->id);
	ADT::Node* a = nullptr;
	if (i == ADT::Node::globalAdt.end())
	{
		n->createIfNotExists();
		a = n.get();
	}
	else
	{
		if (i->second)
		{
			i->second->clone(n);
		}
		else {
			i->second = n;
		}
		a = i->second.get();
	}

	char buf[64];
	sprintf(buf, "%p", a);
	INFO(std::string("ADT UPDATE [") + std::to_string(n->id) + "]: " + buf);
}

void PADTUpdate::onReceived(EntityPlayerMP*)
{
}
