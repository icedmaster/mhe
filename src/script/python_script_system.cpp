#include <python/Python.h>
#include "script/python_script_system.hpp"

namespace mhe {
namespace script {

PythonScriptSystem::~PythonScriptSystem()
{
	close();
}

bool PythonScriptSystem::init()
{
	try
	{
		Py_Initialize();
		boost::python::object main_module = boost::python::import("__main__");
		main_namespace = main_module.attr("__dict__");
		return true;
	}
	catch (...)
	{
		return false;
	}
}

void PythonScriptSystem::close()
{
	Py_Finalize();
}
	
void PythonScriptSystem::exec(const std::string& exp)
{
	boost::python::exec(exp.c_str(), main_namespace, main_namespace);
}

void PythonScriptSystem::exec_file(const std::string& filename)
{
	boost::python::exec_file(filename.c_str(), main_namespace, main_namespace);
}

}}	// namespaces
