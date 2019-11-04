#pragma once
#include <string>
#include <boost/python.hpp>
#include "ModManager.h"
#include "PythonMod.h"
#include <boost/python/suite/indexing/map_indexing_suite.hpp>
#include "Utils.h"

namespace bpy = boost::python;

class CGE_mod
{
public:
	const std::string modid;
	CGE_mod(const std::string& m);

	bpy::object call(bpy::object cl);
};
int getWidth();
int getHeight();
void setTitle(std::string t);
bpy::object getSide();
void registerEventHandler(bpy::object mod, bpy::object handler, std::string name);
bpy::list getPlayers();
bpy::list getWorlds();
void dgs(bpy::object&);
Block* getBlock(_R r);
void cts(std::string url, unsigned short port);
void disconnect();
#if PY_VERSION_HEX >= 0x03000000
extern "C" PyObject* encode_string_unaryfunc2(PyObject* x)
{
	return PyUnicode_FromEncodedObject(x, 0, 0);
}
unaryfunc py_unicode_as_string_unaryfunc = encode_string_unaryfunc2;
#endif
extern "C" PyObject* identity_unaryfunc2(PyObject* x)
{
	Py_INCREF(x);
	return x;
}

// Given a target type and a SlotPolicy describing how to perform a
// given conversion, registers from_python converters which use the
// SlotPolicy to extract the type.
template <class T, class SlotPolicy>
struct slot_rvalue_from_python
{
public:
	slot_rvalue_from_python()
	{
		bpy::converter::registry::insert(
			&slot_rvalue_from_python<T, SlotPolicy>::convertible
			, &slot_rvalue_from_python<T, SlotPolicy>::construct
			, bpy::type_id<T>()
			, &SlotPolicy::get_pytype
		);
	}

private:
	static void* convertible(PyObject* obj)
	{
		unaryfunc* slot = SlotPolicy::get_slot(obj);
		return slot && *slot ? slot : 0;
	}

	static void construct(PyObject* obj, bpy::converter::rvalue_from_python_stage1_data* data)
	{
		// Get the (intermediate) source object
		unaryfunc creator = *static_cast<unaryfunc*>(data->convertible);
		python::handle<> intermediate(creator(obj));

		// Get the location in which to construct
		void* storage = ((bpy::converter::rvalue_from_python_storage<T>*)data)->storage.bytes;
# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable:4244)
# endif 
		new (storage) T(SlotPolicy::extract(intermediate.get()));

# ifdef _MSC_VER
#  pragma warning(pop)
# endif 
		// record successful construction
		data->convertible = storage;
	}
};
unaryfunc py_object_identity = identity_unaryfunc2;
unaryfunc py_encode_string = encode_string_unaryfunc2;
struct string_rvalue_from_python
{
	// If the underlying object is "string-able" this will succeed
	static unaryfunc* get_slot(PyObject* obj)
	{
		return PyUnicode_Check(obj)
			? &py_object_identity
#if PY_VERSION_HEX >= 0x03000000
			: PyBytes_Check(obj)
#else
			: PyString_Check(obj)
#endif
			? &py_encode_string
			: 0;
	};

	// Remember that this will be used to construct the result object 
	static std::string extract(PyObject* intermediate)
	{
		// On Windows, with Python >= 3.3, PyObject_Length cannot be used to get
		// the size of the wchar_t string, because it will count the number of
		// *code points*, but some characters not on the BMP will use two UTF-16
		// *code units* (surrogate pairs).
		// This is not a problem on Unix, since wchar_t is 32-bit.
#if defined(_WIN32) && PY_VERSION_HEX >= 0x03030000
		BOOST_STATIC_ASSERT(sizeof(wchar_t) == 2);

		Py_ssize_t size = 0;
		wchar_t *buf = PyUnicode_AsWideCharString(intermediate, &size);
		if (buf == NULL) {
			bpy::throw_error_already_set();
		}
		std::wstring result(buf, size);
		PyMem_Free(buf);
#else
		std::wstring result(::PyObject_Length(intermediate), L' ');
		if (!result.empty())
		{
			int err = PyUnicode_AsWideChar(
#if PY_VERSION_HEX < 0x03020000
				(PyUnicodeObject *)
#endif
				intermediate
				, &result[0]
				, result.size());

			if (err == -1)
				throw_error_already_set();
		}
#endif
		if (result.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_ACP, 0, &result[0], (int)result.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_ACP, 0, &result[0], (int)result.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}
	static PyTypeObject const* get_pytype() { return &PyUnicode_Type; }
};
 
BOOST_PYTHON_MODULE(cge)
{
	slot_rvalue_from_python<std::string, string_rvalue_from_python>();
	bpy::def("setTitle", &setTitle);
	bpy::class_<CGE_mod>("CGEMod", bpy::init<std::string>(bpy::args("modid")))
		.def("__call__", &CGE_mod::call, bpy::args("target"))
		;


	bpy::class_<PythonEvent>("PythonEvent", bpy::init<_R>(bpy::args("resource")))
		.def("cancel", bpy::make_function(&PythonEvent::cancel))
		.def("__getattr__", bpy::make_function(&PythonEvent::Py_GetAttr, bpy::return_value_policy<bpy::copy_non_const_reference>()))
		;

	bpy::class_<_R>("Resource", bpy::init<std::string>(bpy::args("path")))
		.def_readwrite("full", &Resource::full)
		.def_readwrite("domain", &Resource::domain)
		.def_readwrite("path", &Resource::path)
		;
	bpy::def("registerEventHandler", &registerEventHandler, bpy::args("mod", "callable", "name"));
	bpy::def("color", &Utils::fromHex);
	bpy::class_<Chat>("Chat")
		.def("addChat", &Chat::addChat, bpy::args("m"))
		.def("broadcast", &Chat::broadcast, bpy::args("m"))
		.def("clear", bpy::make_function(&Chat::clear))
		;
	bpy::class_<glm::vec4>("Vec4", bpy::init<float, float, float, float>())
		.def_readwrite("x", &glm::vec4::x)
		.def_readwrite("y", &glm::vec4::y)
		.def_readwrite("z", &glm::vec4::z)
		.def_readwrite("a", &glm::vec4::a)
		;
	bpy::class_<CommandSender, boost::noncopyable>("CommandSender", bpy::no_init)
		.def("sendMessage", bpy::pure_virtual(&CommandSender::sendMessage))
		;
	bpy::class_<PlayerCommandSender, bpy::bases<CommandSender>>("PlayerCommandSender", bpy::no_init)
		.def("getPlayer", bpy::make_function(&PlayerCommandSender::getPlayer, bpy::return_value_policy<bpy::reference_existing_object>()))
		.def("sendMessage", &PlayerCommandSender::sendMessage, bpy::args("message"))
		;

	bpy::class_<NetPlayerHandler>("NetPlayerHandler", bpy::no_init)
		.def_readonly("commandSender", &NetPlayerHandler::commandSender)
		.def("getPlayer", bpy::make_function(&NetPlayerHandler::getPlayer, bpy::return_value_policy<bpy::reference_existing_object>())
		);
	bpy::class_<Block>("Block", bpy::no_init)
		.def("getId", bpy::make_function(&Block::getId, bpy::return_value_policy<bpy::copy_non_const_reference>()))
		.def("getData", bpy::make_function(&Block::getData, bpy::return_value_policy<bpy::return_by_value>()))
		.def("getResource", bpy::make_function(&Block::getResource, bpy::return_value_policy<bpy::return_by_value>()))
		.def("isAir", bpy::make_function(&Block::isAir, bpy::return_value_policy<bpy::return_by_value>()))
		.def("isSolid", bpy::make_function(&Block::isSolid, bpy::return_value_policy<bpy::return_by_value>()))
		.def("isOpaque", bpy::make_function(&Block::isOpaque, bpy::return_value_policy<bpy::return_by_value>()))
		;
	bpy::class_<TileEntity>("TileEntity", bpy::init<Block*>(bpy::args("block")))
		.def("getPos", bpy::make_function(&TileEntity::getPos, bpy::return_value_policy<bpy::copy_const_reference>()))
		.def("getBlock", bpy::make_function(&TileEntity::getBlock, bpy::return_value_policy<bpy::reference_existing_object>()))
		;
	bpy::class_<Entity>("Entity")
		.def("getPos", bpy::make_function(&Entity::getPos, bpy::return_value_policy<bpy::copy_const_reference>()))
		.def("setLook", &Entity::setLook, bpy::args("yaw", "pitch"))
		.def("setPos", &Entity::setPos, bpy::args("pos"))
		.def_readonly("yaw", &Entity::yaw)
		.def_readonly("pitch", &Entity::pitch)
		.def("isOnGround", bpy::make_function(&Entity::isOnGround, bpy::return_value_policy<bpy::return_by_value>()))
		.def("isFlying", bpy::make_function(&Entity::isFlying, bpy::return_value_policy<bpy::return_by_value>()))
		.def("setFlying", &Entity::setFlying, bpy::args("flying"))
		.def("getId", bpy::make_function(&Entity::getId, bpy::return_value_policy<bpy::copy_const_reference>()))
		;

	bpy::class_<GameProfile>("GameProfile", bpy::no_init)
		.def("getUsername", bpy::make_function(&GameProfile::getUsername, bpy::return_value_policy<bpy::copy_const_reference>()))
		;
	bpy::class_<EntityPlayer, bpy::bases<Entity>>("EntityPlayer")
		.def("getGameProfile", bpy::make_function(&EntityPlayer::getGameProfile, bpy::return_value_policy<bpy::copy_const_reference>()))
		;
	bpy::class_<EntityPlayerMP, bpy::bases<EntityPlayer>>("EntityPlayerMP", bpy::no_init)
		.def("getNetHandler", bpy::make_function(&EntityPlayerMP::getNetHandler, bpy::return_value_policy<bpy::reference_existing_object>()))
		;
	bpy::class_<Pos>("Pos", bpy::init<p_type, p_type, p_type>())
		.def_readwrite("x", &Pos::x)
		.def_readwrite("y", &Pos::y)
		.def_readwrite("z", &Pos::z)
		;
	bpy::class_<DPos>("DPos", bpy::init<dtype, dtype, dtype>())
		.def_readwrite("x", &DPos::x)
		.def_readwrite("y", &DPos::y)
		.def_readwrite("z", &DPos::z)

		.def("round", bpy::make_function(&DPos::round, bpy::return_value_policy<bpy::return_by_value>()))
		.def("ceil", bpy::make_function(&DPos::ceil, bpy::return_value_policy<bpy::return_by_value>()))
		.def("floor", bpy::make_function(&DPos::floor, bpy::return_value_policy<bpy::return_by_value>()))
		;
	bpy::class_<World>("World", bpy::no_init)
		.def("getEntity", bpy::make_function(&World::getEntity, bpy::return_value_policy<bpy::reference_existing_object>(), bpy::args("id")))
		.def("getHighestPoint", bpy::make_function(&World::getHighestPoint, bpy::return_value_policy<bpy::return_by_value>(), bpy::args("x", "z")))
		.def("getBlock", bpy::make_function(&World::getBlock, bpy::return_value_policy<bpy::reference_existing_object>(), bpy::args("pos")))
		.def("setBlock", &World::setBlockPython, bpy::args("block", "pos"))
		;

	bpy::scope().attr("CLIENT") = bpy::object(0);
	bpy::scope().attr("SERVER") = bpy::object(1);
	bpy::def("getSide", getSide);
	bpy::scope().attr("chat") = bpy::ptr(CGE::instance->chat);

	bpy::def("getPlayers", bpy::make_function(&getPlayers, bpy::return_value_policy<bpy::return_by_value>()));
	bpy::def("getBlock", bpy::make_function(&getBlock, bpy::return_value_policy<bpy::reference_existing_object>()));

	bpy::def("getWorlds", bpy::make_function(&getWorlds, bpy::return_value_policy<bpy::return_by_value>()));
#ifndef SERVER
	bpy::def("getWidth", bpy::make_function(&getWidth, bpy::return_value_policy<bpy::return_by_value>()));
	bpy::def("getHeight", bpy::make_function(&getHeight, bpy::return_value_policy<bpy::return_by_value>()));
	bpy::def("displayGuiScreen", &dgs, bpy::args("screen"));
	bpy::def("connectToServer", &cts, bpy::args("url", "port"));
	bpy::def("disconnect", &disconnect);
	bpy::class_<Gui>("Gui")
		.def("name", bpy::make_function(&Gui::name, bpy::return_value_policy<bpy::copy_const_reference>()))
		.def_readwrite("x", &Gui::x)
		.def_readwrite("y", &Gui::y)
		.def_readwrite("width", &Gui::width)
		.def_readwrite("height", &Gui::height)
		.def("render", &Gui::render, bpy::args("flags", "mouseX", "mouseY"))
		.def("onMouseClick", &Gui::render, bpy::args("mouseX", "mouseY", "button"))
		;
	
	bpy::class_<GuiScreen, bpy::bases<Gui>, boost::shared_ptr<GuiScreen>>("GuiScreen")
		.def("render", &GuiScreen::render, bpy::args("flags", "mouseX", "mouseY"))
		.def("renderWidgets", &GuiScreen::renderWidgets, bpy::args("flags", "mouseX", "mouseY"))
		.def("onMouseClick", &GuiScreen::render, bpy::args("mouseX", "mouseY", "button"))
		.def("close", &GuiScreen::close)
		;
	bpy::class_<GuiScreenMessageDialog, bpy::bases<GuiScreen>, boost::shared_ptr<GuiScreenMessageDialog>>("GuiScreenMessageDialog", bpy::init<std::string, std::string, bpy::object, uint8_t>(bpy::args("title", "text", "callback", "type")))
		.def("render", &GuiScreenMessageDialog::render, bpy::args("flags", "mouseX", "mouseY"))
		.def("renderWidgets", &GuiScreenMessageDialog::renderWidgets, bpy::args("flags", "mouseX", "mouseY"))
		.def("onMouseClick", &GuiScreenMessageDialog::render, bpy::args("mouseX", "mouseY", "button"))
		.def("close", &GuiScreenMessageDialog::close)
		;
	bpy::class_<GameRenderer>("GameRenderer")
		.def("drawRect", &GameRenderer::drawRectNUV, bpy::args("x", "y", "width", "height"))
		.def("drawRect", &GameRenderer::drawRectUV, bpy::args("x", "y", "width", "height", "u", "v"))
		.def("drawRect", &GameRenderer::drawRectUVUV, bpy::args("x", "y", "width", "height", "u", "v", "u2", "v2"))
		.def("setTexturingMode", &GameRenderer::setTexturingMode, bpy::args("mode"))
		.def("setColor", &GameRenderer::setColor, bpy::args("c"))
		;
	bpy::class_<TextureManager>("TextureManager")
		.def("bind", &TextureManager::bind, bpy::args("t"))
		.def("load", &TextureManager::load2, bpy::args("t", "smooth"))
		;
	bpy::scope().attr("renderer") = bpy::object(bpy::ptr(CGE::instance->renderer));
	bpy::scope().attr("textureManager") = bpy::object(bpy::ptr(CGE::instance->textureManager));
#endif 
}