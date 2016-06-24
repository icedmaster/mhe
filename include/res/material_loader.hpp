#ifndef __MATERIAL_LOADER_HPP__
#define __MATERIAL_LOADER_HPP__

#include "core/compiler.hpp"
#include "core/string.hpp"
#include "render/material.hpp"

namespace mhe {

struct Context;
struct MaterialData;

struct MaterialLoader
{
	typedef MaterialData::IdType type;
    typedef MaterialData::IdType instance_type;
	typedef Context context_type;
    static bool MHE_EXPORT load(type& res, const FilePath& name, const context_type* context);
};

}

#endif
