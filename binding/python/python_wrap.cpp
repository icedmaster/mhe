#include "mhe_math_python_wrap.hpp"
#include <boost/python.hpp>

BOOST_PYTHON_MODULE(_mhe)
{
	mhe::wrap::create_math_wrap();
}
