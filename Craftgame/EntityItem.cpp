#include "EntityItem.h"
#include "EntityPlayer.h"
#include "GameEngine.h"
#include <boost/smart_ptr/make_shared.hpp>
#ifndef SERVER
#include "GameEngine.h"
#endif

EntityItem::~EntityItem()
{
	if (item)
		delete item;
}

void EntityItem::tick()
{
	EntityPhysics::tick();
	if (!worldObj->isRemote)
	{
		if (t)
		{
			t--;
		}
		else {
			AABB a = createAABB();
			for (Entity*& e : worldObj->entities)
			{
				if (e != this)
				{
					if (a.test(e->createAABB()))
						if (EntityPlayer* pl = dynamic_cast<EntityPlayer*>(e))
						{
							if (pl->inventory) {
								size_t c = 0;
								for (Slot*& s : pl->inventory->slots)
								{
									if (s->stack && s->stack->getItem() == item->getItem()) {
										pl->inventory->put(c, item->getCount(), item);
										worldObj->playSound("pop", getPos());
										if (!item)
										{
											setDead();
											return;
										}
									}
									c++;
								}
								c = 0;
								for (Slot*& s : pl->inventory->slots)
								{
									if (!s->stack) {
										pl->inventory->put(c, item->getCount(), item);
										worldObj->playSound("pop", getPos());
										if (!item)
										{
											setDead();
											return;
										}
									}
									c++;
								}
							}
						}
				}
			}
		}
	}
}

void EntityItem::render()
{
#ifndef SERVER
	Entity::render();
	if (item)
	{
		glm::mat4 t = getTransform();
		t = glm::scale(t, glm::vec3(0.4f));
		t = glm::translate(t, glm::vec3(0, cosf(_anim * 20 / 3.14) * 0.3f + 0.3f, 0));
		t = glm::rotate(t, glm::radians(_anim * 360.f), glm::vec3(0, 1, 0));
		CGE::instance->renderer->setTransform(t);
		item->getItem()->getRenderer()->render(IItemRenderer::DROP, item->getItem(), item);
		_anim += CGE::instance->millis * 0.2f;
		_anim = fmodf(_anim, 1.f);
	}
#endif
}

void EntityItem::unpack()
{
	Entity::unpack();
	boost::shared_ptr<ADT::Node> n = adtTag->getNode("item");
	if (n)
	{
		if (boost::shared_ptr<ADT::Value> val = n->toValue())
		{
			ByteBuffer buf;
			val->toByteBuffer(buf);
			buf.seekg(0);
			buf >> item;
		}
	}
}

void EntityItem::pack()
{
	Entity::pack();
	boost::shared_ptr<ADT::Value> val = boost::make_shared<ADT::Value>();
	ByteBuffer buf;
	buf << item;
	buf.seekg(0);
	val->setValueBuffer(buf);
	adtTag->putNode("item", val);
}

bool EntityItem::onCollideWith(Entity* other)
{
	return !dynamic_cast<EntityPlayer*>(other);
}

void EntityItem::onVelocityChanged(glm::vec3 m)
{
	if (!worldObj->isRemote && glm::length(m) > 12.f)
	{
		if (item->getCount() > 2)
		{
			ItemStack* cloned = item->getItem()->newStack();
			cloned->setCount(item->getCount() / 2);
			item->setCount(item->getCount() - cloned->getCount());
			EntityItem* e = new EntityItem(worldObj, CGE::instance->assignGlobalUniqueEntityId(), cloned);
			worldObj->spawnEntity(e, getPos());
			e->motion.x = (rand() % 2000) / 1000.f - 1;
			e->motion.z = (rand() % 2000) / 1000.f - 1;
			e->motion.y = (rand() % 700) / 1000.f;
			e->motion *= (glm::length(m) + m) / 3.f;
			motion.x = (rand() % 2000) / 1000.f - 1;
			motion.z = (rand() % 2000) / 1000.f - 1;
			motion.y = (rand() % 700) / 1000.f;
			motion *= (glm::length(m) + m) / 3.f;
		}
	}
}

