#include "PythonMod.h"
#include "GameEngine.h"
#include "File.h"

PythonMod::PythonMod(boost::python::object cl) {
	handle = cl;
	mFile = CGE::instance->modManager->currentFile;
}