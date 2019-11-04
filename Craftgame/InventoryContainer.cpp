#include "InventoryContainer.h"
#include "GameEngine.h"
#include "PContainerAction.h"

/**
 * \brief Обмен предметами
 * \param index Слот
 * \param stack Предмет
 * \return Предыдущий предмет в слоте
 */
ItemStack* InventoryContainer::swap(size_t index, ItemStack* stack)
{
	index %= slots.size();
	return _swap(index, stack);
}
ItemStack* InventoryContainer::_swap(size_t index, ItemStack* stack)
{

	ItemStack* prev = slots[index]->stack;
	slots[index]->stack = stack;
	return prev;
}

bool InventoryContainer::get(size_t index, uint16_t count, ItemStack*& dst)
{
	if (count == 0)
		count++;
	if (index >= slots.size())
		return false;
	Slot*& s = slots[index];
	if (!s->stack)
		return false;
	if (dst)
	{
		if (dst->getItem() == s->stack->getItem())
		{
			if (s->stack->getCount() == count)
			{
				dst->setCount(dst->getCount() + count);
				delete s->stack;
				s->stack = nullptr;
				return true;
			} else if (s->stack->getCount() > count)
			{
				s->stack->setCount(s->stack->getCount() - count);
				dst->setCount(dst->getCount() + count);
				return true;
			}
			else
			{
				return false;
			}
		}
	} else
	{
		if (s->stack->getCount() == count)
		{
			dst = _swap(index, nullptr);
			return true;
		} else if (s->stack->getCount() > count)
		{
			dst = s->stack->getItem()->newStack();
			dst->setCount(count);
			s->stack->setCount(s->stack->getCount() - count);
			return true;
		} else
		{
			return false;
		}
	}
	return false;
}

bool InventoryContainer::put(size_t index, uint16_t count, ItemStack*& from)
{
	if (index >= slots.size())
		return false;
	Slot*& s = slots[index];
	if (from)
	{
		if (s->stack)
		{
			if (s->stack->getItem() == from->getItem())
			{
				uint16_t total = s->stack->getCount() + count;
				if (total > s->stack->getItem()->getMaxStackCount())
				{
					uint16_t spent = s->stack->getItem()->getMaxStackCount() - s->stack->getCount();
					from->setCount(from->getCount() - spent);
					s->stack->setCount(s->stack->getItem()->getMaxStackCount());
					return true;
				}
				else {
					s->stack->setCount(s->stack->getCount() + count);
					from->setCount(from->getCount() - count);
					if (from->getCount() <= 0)
					{
						delete from;
						from = nullptr;
					}
					return true;
				}
			} else
			{
				ItemStack* k = s->stack;
				s->stack = from;
				from = k;
			}
		}
		else
		{
			if (from->getCount() == count)
			{
				s->stack = from;
				from = nullptr;
				return true;
			}
			else {
				s->stack = new ItemStack(*from);
				s->stack->setCount(1);
				from->setCount(from->getCount() - 1);
				if (from->getCount() == 0)
				{
					delete from;
					from = nullptr;
				}
				return true;
			}
		}
	}
	return false;
}


InventoryContainer::~InventoryContainer()
{
	for (Slot*& s : slots)
		delete s;
}