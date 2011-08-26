#include "mhe_math_python_wrap.hpp"
#include <boost/python.hpp>

using namespace boost::python;

namespace
{
	void create_vector2_wrap();
	void create_vector3_wrap();
	void create_vector4_wrap();
	void create_rect_wrap();
	void wrap_utils();
}

void mhe::wrap::create_math_wrap()
{
	create_vector2_wrap();
	create_vector3_wrap();
	create_vector4_wrap();
	create_rect_wrap();
	wrap_utils();
}

namespace
{
	void create_vector2_wrap()
	{		
		typedef mhe::vector2<float> v2d;
	
		void (v2d::*set1)(float) = &v2d::set;
		void (v2d::*set2)(float, float) = &v2d::set;
		class_<v2d>("v2d", init<float, float>())
		.def(init<>())	
		.def("set", set1)
		.def("set", set2)
		.def("x", &v2d::x)
		.def("y", &v2d::y)
		.def("set_x", &v2d::set_x)
		.def("set_y", &v2d::set_y);
	}
	
	void create_vector3_wrap()
	{		
		typedef mhe::vector3<float> v3d;
	
		void (v3d::*set1)(float) = &v3d::set;
		void (v3d::*set2)(float, float, float) = &v3d::set;
		class_<v3d>("v3d", init<float, float, float>())
		.def(init<>())	
		.def("set", set1)
		.def("set", set2)
		.def("x", &v3d::x)
		.def("y", &v3d::y)
		.def("z", &v3d::z)
		.def("length", &v3d::length);
	}
	
	void create_vector4_wrap()
	{		
		typedef mhe::vector4<float> v4d;
	
		void (v4d::*set1)(float) = &v4d::set;
		void (v4d::*set2)(float, float, float, float) = &v4d::set;
		class_<v4d>("v4d", init<float, float, float, float>())
		.def(init<>())	
		.def("set", set1)
		.def("set", set2)
		.def("x", &v4d::x)
		.def("y", &v4d::y)
		.def("z", &v4d::z)
		.def("w", &v4d::w)
		.def("r", &v4d::r)
		.def("g", &v4d::g)
		.def("b", &v4d::b)
		.def("a", &v4d::a);		
	}
	
	void create_rect_wrap()
	{
		typedef mhe::rect<float> rectf;				
		
		class_<rectf>("rectf", init<float, float, float, float>())
		.def(init<>())
		.def(init<mhe::v2d, mhe::v2d>())
		.def("set", &rectf::set)
		.def("width", &rectf::width)
		.def("height", &rectf::height)
		.def("ll", &rectf::ll, return_value_policy<copy_const_reference>())
		.def("rh", &rectf::rh, return_value_policy<copy_const_reference>());		
	}
	
	void wrap_utils()
	{
		def("deg_to_rad", mhe::deg_to_rad);
		def("is_pot", mhe::is_pot);
		def("nearest_pot", mhe::nearest_pot);
	}
}

BOOST_PYTHON_MODULE(_mhe_math)
{
	mhe::wrap::create_math_wrap();
}