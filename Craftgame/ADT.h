#pragma once
#include <string>
#include <map>
#include <vector>
#include "ByteBuffer.h"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace ADT {
	class Value;
	class Map;
	class ADTString;

	enum ADT_TYPE

	{
		VALUE = 0x00,
		STRING = 0x01,
		MAP = 0x02
	};
	enum SyncMode
	{
		SYNC_ENABLED = 0x04
	};

	class Node: public boost::enable_shared_from_this<Node>
	{
	public:


		static std::map<size_t, boost::shared_ptr<Node>> globalAdt;
		size_t id;
		boost::shared_ptr<Node> parent = nullptr;

		int meta;
		Node(ADT_TYPE t, bool sync = false);
		virtual ~Node();
		boost::shared_ptr<Value> toValue();
		boost::shared_ptr<Map> toMap();
		boost::shared_ptr<ADTString> toADTString();
		std::string& toString();
		void setId(const size_t id);
		void valueUpdated();
		virtual void serialize(ByteBuffer& b) = 0;
		virtual void deserialize(ByteBuffer& b) = 0;
		virtual void clone(boost::shared_ptr<Node> node) = 0;
		boost::shared_ptr<Node> getSharedPtr();
		void createIfNotExists();
	};

	class Value : public Node
	{
	private:
		char* buffer = nullptr;
		size_t size = 0;
	public:

		Value(bool s = false): Node(VALUE, s) {}

		Value(const Value&);
		virtual ~Value();

		template<typename T>
		T getValue()
		{
			return *reinterpret_cast<T*>(buffer);
		}
		template<typename T>
		boost::shared_ptr<Value> setValue(const T& t)
		{
			if (buffer)
				delete[] buffer;
			buffer = new char[sizeof(t)];
			memcpy(buffer, reinterpret_cast<const char*>(&t), sizeof(T));
			size = sizeof(T);
			valueUpdated();
			return getSharedPtr()->toValue();
		}
		void setValueBuffer(const ByteBuffer& v);
		void toByteBuffer(ByteBuffer& b);
		virtual void serialize(ByteBuffer& b);
		virtual void deserialize(ByteBuffer& b);
		virtual void clone(boost::shared_ptr<Node> node);
	};

	class Map : public Node
	{
	private:
		std::map<std::string, boost::shared_ptr<Node>> _data;
	public:
		Map(bool s = false) : Node(MAP, s) {}
		Map(const Map& m);
		virtual ~Map();
		boost::shared_ptr<Node> getNode(std::string name);
		void putNode(std::string name, boost::shared_ptr<Node> value);
		void removeNode(std::string name);
		virtual void serialize(ByteBuffer& b);
		virtual void deserialize(ByteBuffer& b);
		virtual void clone(boost::shared_ptr<Node> node) override;
	};

	class ADTString : public Node
	{
	private:
		std::string _h;
	public:
		ADTString(const ADTString& m);
		ADTString(const char* t, bool s = false) :
			Node(STRING, s),
			_h(t)
		{

		}
		ADTString(const std::string& b, bool s = false) :
			Node(STRING, s),
			_h(b)
		{

		}
		std::string& toString()
		{
			return _h;
		}
		ADTString(): Node(STRING) {}
		virtual void serialize(ByteBuffer& b);
		virtual void deserialize(ByteBuffer& b);
		virtual void clone(boost::shared_ptr<Node> node) override;
	};


	void serialize(ByteBuffer&, boost::shared_ptr<Node>);
	boost::shared_ptr<Node> deserialize(ByteBuffer&, bool callCreate = true);
};