#include "Items.h"
#include "GameEngine.h"
#include "ModelItemRenderer.h"
#include "FbxModelItemRenderer.h"
#include "ItemAnimator.h"
#include "ItemCharging.h"
#include "Utils.h"
#include "EntityYoYo.h"
#include "TrailFX.h"
#include <boost/smart_ptr/make_shared.hpp>

extern bool __3rd_person;

void Items::init()
{
	CGE::instance->goRegistry->registerGO(new Item(_R("items/stick"), 8, 0, new ModelItemRenderer("stick.obj", CGE::instance->materialRegistry->getMaterial("stick"))));
	{
		class a: public ItemAnimator, public ItemCharging
		{
		public:

			class stack: public ItemStackAnimatable, public IStackCharging
			{
			public:

				stack(Item* i)
					: ItemStackAnimatable(i)
				{
					setADT(boost::make_shared<ADT::Map>(true));
				}
				virtual ~stack()
				{
					if (source)
					{
						delete source;
						source = nullptr;
					}
				}
				EntityPlayerMP* player = nullptr;
				LightSource* source = nullptr;
				glm::vec3 prevPos1; // 1 person
				glm::vec3 prevPos2; // 3 person
				EntityYoYo* yoyo = nullptr;
				bool rightClick = false;
				float distance = -1.f;
			};
			class r: public FbxModelItemRenderer
			{
			private:
				float anim = 0.f;
			public:

				r(const std::string& model, Material* m)
					: FbxModelItemRenderer(model, m)
				{
				}

				virtual void render(Type t, Item* item, ItemStack* stack) override {
#ifndef SERVER
					if (boost::shared_ptr<ADT::Map> m = stack->getRawADT())
					{
						if (m->getNode("h"))
						{
							return;
						}
					}
					CGE::instance->renderer->setTexturingMode(1);
					CGE::instance->renderer->setColor(glm::vec4(1.f));
					mMaterial->bind();
					std::map<std::string, glm::mat4> tr;
					std::map<std::string, SkeletalAnimation> kek;
					if (IAnimatable* k = dynamic_cast<IAnimatable*>(stack)) {
						std::map<std::string, SkeletalAnimation>* sk = nullptr;
						k->animate(sk, tr);
						if (t != INVENTORY) {
							IStackCharging* j = dynamic_cast<IStackCharging*>(stack);
							if (j)
							{
								if (j->isUsed)
								{
									float ra = glm::clamp(float(j->ticksCharging) / 360.f, 0.f, 1.f);


									tr["base"] = glm::translate(glm::rotate(glm::translate(glm::mat4(1.0), glm::vec3(-0.8f * ra, 0, 0)), glm::radians(anim), glm::vec3(1, 0, 0)), glm::vec3(0, 1.5f * ra, 0));
									//tr["base"] = glm::translate(glm::mat4(1.0), glm::vec3(5, 0, 0));
									if ((CGE::instance->renderer->renderType & GameRenderer::RENDER_FIRST) && t == ENTITY) {
										anim += CGE::instance->millis * powf(ra * 2.5f, 2.f) * 360.f;
										anim = fmodf(anim, 360.f);
									}


									a::stack* st = dynamic_cast<a::stack*>(stack);
									if (st->source && st->player)
									{
										st->source->pos = glm::vec3(glm::rotate(CGE::instance->renderer->transform, glm::radians(90.f), glm::vec3(0, 1, 0)) * tr["base"] * glm::vec4(t == ENTITY ? -0.9 : -6.f, (t == ENTITY ? 2.2f : 6.f) * powf(ra, 2.f), 0, 1));
										st->source->color = glm::vec3(Utils::fromHex(0xfd6eabff)) * ra * 8.f;
									}
									if (CGE::instance->renderer->renderFlags & GameRenderer::RENDER_DEFAULT) {
										if (st->player != CGE::instance->thePlayer || (__3rd_person && t != FIRST_PERSON) || (!__3rd_person && t == FIRST_PERSON)) {
											glm::vec3 prev = t == ENTITY ? st->prevPos2 : st->prevPos1;
											if (prev != glm::vec3(0))
												CGE::instance->thePlayer->worldObj->customParticles.push_back(new TrailFX(st->source->pos, prev, { 1, 1, 1, 1 }));
										}
									}
									if (t == ENTITY)
									{
										st->prevPos2 = st->source->pos;
									} else
									{
										st->prevPos1 = st->source->pos;
									}
								}
							}
						}
						CGE::instance->renderer->setTransform(glm::scale(CGE::instance->renderer->transform, glm::vec3(2.2f)));

						if (t == FIRST_PERSON) {
							int x = CGE::instance->renderer->renderType;
							CGE::instance->renderer->renderType |= GameRenderer::RENDER_FIRST;

							CGE::instance->renderer->renderModelAssimp(mModel, sk, tr, false);
							CGE::instance->renderer->renderType = x;
						}
						else
						{
							CGE::instance->renderer->renderModelAssimp(mModel, sk, tr, false);
						}
					}
					else
					{
						CGE::instance->renderer->renderModelAssimp(mModel, nullptr, tr, false);
					}
					mModel->nextFrame();
#endif
				}
			};
			a(const _R& r, bid id, byte data, IItemRenderer* renderer)
				: ItemAnimator(r, id, data, renderer), ItemCharging()
			{
				
			}
			virtual ~a() = default;

			virtual void animate(ItemStackAnimatable* s) override {
				s->animatronic.updateAnimation("idle");
				if (dynamic_cast<IStackCharging*>(s)->isUsed)
				{
					s->animatronic.updateAnimation("ring", 1);
				} else
				{
					s->animatronic.updateAnimation("ring", 0);
				}
			}

			virtual ItemStack* newStack() override {
				return new stack(this);
			}

			virtual int getTF() override {
				return ~T_INVENTORY;
			}

			virtual void startCharging(ItemStack* s, EntityPlayerMP* mp) override {
				stack* st = dynamic_cast<stack*>(s);
				if (st->getADT()->getNode("h")) {
					id_type d = st->getADT()->getNode("h")->toValue()->getValue<id_type>();
					if (Entity* e = mp->worldObj->getEntity(d))
					{
						st->yoyo = dynamic_cast<EntityYoYo*>(e);
						return;
					}
				}
				ItemCharging::startCharging(s, mp);
				if (!st->source)
				{
					st->source = new LightSource(glm::vec3(0.f), glm::vec3(0.f), 30.f);
				}
				st->player = mp;
			}

			virtual uint16_t getMaxStackCount() override {
				return 1;
			}

			virtual void stopCharging(ItemStack* s, EntityPlayerMP* mp) override {
				ItemCharging::stopCharging(s, mp);
				stack* st = dynamic_cast<stack*>(s);
				if (!mp->worldObj->isRemote)
				{
					ItemStack* kek = mp->inventory->slots[mp->getItemIndex()]->stack;
					if (kek && mp->inventory->slots[mp->getItemIndex()]->stack->getItem() == CGE::instance->goRegistry->getGO("items/yoyo")) {
						if (st->ticksCharging > 20)
						{
							st->yoyo = new EntityYoYo(mp);
							st->yoyo->setId(CGE::instance->assignGlobalUniqueEntityId());
							boost::shared_ptr<ADT::Map> m = kek->getADT();
							m->putNode("h", boost::make_shared<ADT::Value>(true)->setValue(st->yoyo->getId()));
							st->yoyo->adtTag->putNode("o", boost::make_shared<ADT::Value>(true)->setValue(mp->getId()));
							mp->worldObj->spawnEntity(st->yoyo, mp->getEyePos());
							st->yoyo->applyMotion(mp->getLookVec() * glm::min(float(st->ticksCharging) / 2.f + 5.f, 50.f));
							st->distance = -1.f;
						}
					}
				}
				st->prevPos2 = st->prevPos1 = glm::vec3(0, 0, 0);
				if (st->source)
				{
					delete st->source;
					st->source = nullptr;
				}
				st->rightClick = false;
			}
			virtual bool itemLeftClick(EntityPlayerMP* sabj, ItemStack* s) override {
				if (!sabj->worldObj->isRemote)
				{
					stack* st = dynamic_cast<stack*>(s);
					st->rightClick = true;
					if (st->yoyo)
					{
						if (!st->yoyo->physics)
						{
							st->yoyo->setPos(glm::normalize(sabj->getPos().toVec3() - st->yoyo->getPos().toVec3()) + st->yoyo->getPos());
						}
						st->yoyo->returnToPlayer();
						st->yoyo->applyMotion(glm::normalize(sabj->getPos().toVec3() - st->yoyo->getPos().toVec3()) * 12.f);
					}
					return false;
				}
				return true;
			}

			virtual bool itemRightClick(EntityPlayerMP* sabj, ItemStack* s) override {
				if (!sabj->worldObj->isRemote)
				{
					stack* st = dynamic_cast<stack*>(s);
					st->rightClick = true;
					if (st->yoyo)
					{
						st->distance = glm::max(glm::length(sabj->getPos().toVec3() - st->yoyo->getPos().toVec3()) - 5.f, 0.f);
						st->yoyo->everBeenInBlock = true;
					}
					return false;
				}
				return true;
			}

			virtual void tick(ItemStack* s, EntityPlayerMP* sabj) override {
				ItemAnimator::tick(s, sabj);
				ItemCharging::tick(s, sabj);
				//CGE::instance->setDebugString("ticks", std::to_string(dynamic_cast<a::stack*>(stack)->ticksCharging));
				if (!sabj->worldObj->isRemote) {
					stack* st = dynamic_cast<stack*>(s);
					if (st->yoyo && st->distance >= 0.f)
					{
						glm::vec3 toPlayer = sabj->getPos().toVec3() - st->yoyo->getPos().toVec3();
						float len = glm::length(toPlayer);
						if (len > 150.f)
						{
							st->yoyo->returnToPlayer();
						} else if (len > st->distance)
						{
							if (st->yoyo->physics)
								st->yoyo->applyMotion(glm::normalize(toPlayer) / 5.f * (len - st->distance));
							else if (st->yoyo->hookedInto)
								st->yoyo->hookedInto->applyMotion(glm::normalize(toPlayer) * .4f * (len - st->distance));
							else
								sabj->applyMotion(glm::normalize(-toPlayer) * .4f * (len - st->distance));
						}
					}
					if (st->rightClick && st->yoyo && st->yoyo->createAABB().test(sabj->createAABB()))
					{
						st->yoyo->setDead();
						st->yoyo = nullptr;
						s->getADT()->removeNode("h");
					}
				}
			}
		};
		CGE::instance->goRegistry->registerGO(new a(_R("items/yoyo"), 11, 0, new a::r("yoyo.fbx", CGE::instance->materialRegistry->getMaterial("yoyo"))));
	}
}
