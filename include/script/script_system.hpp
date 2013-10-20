#ifndef __SCRIPT_SYSTEM_HPP__
#define __SCRIPT_SYSTEM_HPP__

#include <string>

namespace mhe {
namespace script {

template <class ConcreteScriptSystem>
class ScriptSystem
{
public:
	bool init()
	{
		return script_system_impl.init();
	}

	void close()
	{
		script_system_impl.close();
	}

	template <class T>
	void set_global(const T& var, const std::string& name)
	{
		script_system_impl.set_global<T>(var, name);
	}

	template <class T>
	T get_global(const std::string& name)
	{
		return script_system_impl.get_global<T>(name);
	}

	void del_global(const std::string& name)
	{
		script_system_impl.del_global(name);
	}

	void exec(const std::string& value)
	{
		script_system_impl.exec(value);
	}

	void exec_script(const std::string& filename)
	{
		script_system_impl.exec_script(filename);
	}
private:
	ConcreteScriptSystem script_system_impl;
};

}}	// namespaces

#endif	// __SCRIPT_SYSTEM_HPP__
