#ifndef __MATERIAL_LOADER_HPP__
#define __MATERIAL_LOADER_HPP__

#include "material.hpp"

namespace mhe {

class Context;

struct MaterialLoader
{
	typedef Material type;
	typedef Context helper_type;
	static type* load(const std::string& name, const helper_type* helper);
};

}

#endif
