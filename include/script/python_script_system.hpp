#ifndef __PYTHON_SCRIPT_SYSTEM_HPP__
#define __PYTHON_SCRIPT_SYSTEM_HPP__

#include <string>
#include <boost/python.hpp>

namespace mhe {
namespace script {

class PythonScriptSystem
{						
public:	
	~PythonScriptSystem();
	bool init();
	void close();
			
	template <class T>
	void set_global(const T& var, const std::string& name)
	{
		main_namespace[name] = var;
	}
			
	template <class T>
	T get_global(const std::string& name) const
	{
		return boost::python::extract<T>(main_namespace[name.c_str()]);
	}
			
	void del_global(const std::string& name)
	{
		exec("del " + name + "\n");
	}
			
	void exec(const std::string& exp);
	void exec_file(const std::string& filename);
private:
	boost::python::object main_namespace;
};

}}

#endif	// __PYTHON_SCRIPT_SYSTEM_HPP__
