#ifndef __TEST_PYTHON_HPP__
#define __TEST_PYTHON_HPP__

#include "test_common.hpp"
#include "script/python_script_system.hpp"

TEST(TestPython, number_test)
{
	mhe::script::PythonScriptSystem ss;
	ASSERT_TRUE(ss.init());
	std::cout << "initialization successfull" << std::endl;
	// try to set value
	ss.set_global<int>(42, "a");
	std::cout << "set successfull" << std::endl;
	// evaluate expression...
	ss.exec("a = a + 1\n");
	std::cout << "exec successfull" << std::endl;
	int new_a = ss.get_global<int>("a");
	ASSERT_EQ(43, new_a);
}

#endif
