#include "ADT.h"
#include "GameEngine.h"
#include "PADTUpdate.h"
#include "CraftgameServer.h"
#include <boost/smart_ptr/make_shared.hpp>

using namespace ADT;

std::map<size_t, boost::shared_ptr<Node>> Node::globalAdt;
size_t adt_global_id = 1;

Node::Node(ADT_TYPE t, bool sync)
{
	bool v;
#ifdef SERVER
	v = true;
#else
	v = CGE::instance->thePlayer && CGE::instance->thePlayer->worldObj && !CGE::instance->thePlayer->worldObj->isRemote;
#endif
	meta = t;
	if (sync)
	{
		meta |= SYNC_ENABLED;
	}
	id = 0;
	if (v) {
		for (std::map<size_t, boost::shared_ptr<Node>>::iterator i = globalAdt.begin(); i != globalAdt.end(); i++)
		{
			if (!i->second)
			{
				id = i->first;
				return;
			}
		}
	}

}

Node::~Node()
{
}

boost::shared_ptr<Value> Node::toValue()
{
	return boost::dynamic_pointer_cast<Value, Node>(getSharedPtr());
}

boost::shared_ptr<Map> Node::toMap()
{
	return boost::dynamic_pointer_cast<Map, Node>(getSharedPtr());
}

boost::shared_ptr<ADTString> Node::toADTString()
{
	return boost::dynamic_pointer_cast<ADTString, Node>(getSharedPtr());
}

std::string& Node::toString()
{
	return dynamic_cast<ADTString*>(this)->toString();
}

Map::~Map()
{
	_data.clear();
}

boost::shared_ptr<Node> Map::getNode(std::string name)
{
	std::map<std::string, boost::shared_ptr<Node>>::iterator f = _data.find(name);
	if (f == _data.end())
	{
		return nullptr;
	}
	return f->second;
}
void Node::valueUpdated()
{
	if (meta & SYNC_ENABLED) {
		if (id == 0)
			createIfNotExists();
		ts<IServer>::r t = CGE::instance->server;
		if (CGE::instance->server.isValid() && dynamic_cast<CraftgameServer*>(t.get()))
		{
			t->sendPacket(new PADTUpdate(getSharedPtr()));
		}
	}	
}
void Map::putNode(std::string name, boost::shared_ptr<Node> value)
{
	value->parent = getSharedPtr();
	_data[name] = value->getSharedPtr();
	valueUpdated();
}

void Map::removeNode(std::string name)
{
	std::map<std::string, boost::shared_ptr<Node>>::iterator it = _data.find(name);
	if (it != _data.end())
	{
		_data.erase(it);
		valueUpdated();
	}
}

void Map::serialize(ByteBuffer& b)
{
	size_t count = _data.size();
	b << id;
	b << count;
	for (std::map<std::string, boost::shared_ptr<Node>>::iterator i = _data.begin(); i != _data.end(); i++)
	{
		std::string s = i->first;
		b << s;
		ADT::serialize(b, i->second);
	}
}

void Map::deserialize(ByteBuffer& b)
{
	size_t count;
	b >> id;
	b >> count;
	for (size_t i = 0; i < count; i++)
	{
		std::string key;
		b >> key;
		boost::shared_ptr<Node> n = ADT::deserialize(b);
		n->parent = getSharedPtr();
		_data[key] = n;
	}
}

void Map::clone(boost::shared_ptr<Node> node)
{
	if (boost::shared_ptr<Map> n = node->toMap())
	{
		_data = n->_data;
	}
}

ADTString::ADTString(const ADTString& m):
	Node(STRING, m.meta & SYNC_ENABLED),
	_h(m._h)
{
}

void ADTString::serialize(ByteBuffer& b)
{
	b << id;
	b << _h;
}

void ADTString::deserialize(ByteBuffer& b)
{
	b >> id;
	b >> _h;
}

void ADTString::clone(boost::shared_ptr<Node> node)
{
	if (boost::shared_ptr<ADTString> s = node->toADTString())
	{
		_h = s->_h;
	}
}

void ADT::serialize(ByteBuffer& b, boost::shared_ptr<Node> node)
{
	node->createIfNotExists();
	b << node->meta;
	node->serialize(b);
}

boost::shared_ptr<Node> ADT::deserialize(ByteBuffer& b, bool callCreate)
{
	int t;
	int m;
	b >> m;
	t = m & ~(SYNC_ENABLED);
	boost::shared_ptr<Node> node;

	switch (t)
	{
	default:
	case VALUE:
		node = boost::make_shared<Value>();
		break;
	case MAP:
		node = boost::make_shared<Map>();
		break;
	case STRING:
		node = boost::make_shared<ADTString>();
		break;
	}
	node->deserialize(b);
	node->meta = m;
	if (callCreate)
	{
		if (auto n = Node::globalAdt[node->id])
		{
			return n;
		}
		if (m & SYNC_ENABLED)
		{
			node->createIfNotExists();
		}
	}
	return node;
}

Value::Value(const Value& t):
	Node(VALUE, t.meta & SYNC_ENABLED),
	size(t.size)
{
	if (buffer)
		delete[] buffer;
	buffer = new char[size];
	memcpy(buffer, t.buffer, size);
}

Value::~Value()
{
	if (buffer)
		delete[] buffer;
}

void ADT::Value::setValueBuffer(const ByteBuffer & v)
{
	size = v.size();
	if (buffer)
		delete[] buffer;
	buffer = new char[size];
	memcpy(buffer, v.get(), size);
	valueUpdated();
}

void ADT::Value::toByteBuffer(ByteBuffer & b)
{
	b.put(buffer, size);
}

void Value::serialize(ByteBuffer& b)
{
	b << id;
	b << size;
	b.put(buffer, size);
}

void Value::deserialize(ByteBuffer& b)
{
	b >> id;
	b >> size;
	buffer = new char[size];
	b.get(buffer, size);
}

void Value::clone(boost::shared_ptr<Node> node)
{
	if (auto n = node->toValue())
	{
		if (buffer)
			delete[] buffer;
		buffer = new char[size = n->size];
		memcpy(buffer, n->buffer, size);
	}
}

Map::Map(const Map& m):
	Node(MAP, m.meta & SYNC_ENABLED),
	_data(m._data)
{
	
}

void Node::setId(const size_t _id)
{
	id = _id;
}
void Node::createIfNotExists()
{
	if (meta & SYNC_ENABLED) {
		if (id == 0)
		{
			id = adt_global_id++;
		}
		if (globalAdt[id] == nullptr)
		{
			char buf[64];
			sprintf(buf, "prev %p new %p", globalAdt[id].get(), this);
			globalAdt[id] = getSharedPtr();
			INFO(std::string("ADT id ") + std::to_string(id) + " updated " + buf);
		} else
		{
			WARN(std::string("ADT with id ") + std::to_string(id) + " is already exists");
		}
	}
}

boost::shared_ptr<Node> Node::getSharedPtr()
{
	return shared_from_this();
}
