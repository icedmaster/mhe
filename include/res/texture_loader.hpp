#ifndef __TEXTURE_LOADER_HPP__
#define __TEXTURE_LOADER_HPP__

#include "core/compiler.hpp"
#include "core/string.hpp"

namespace mhe {

struct Context;
struct TextureInstance;

struct TextureLoader
{
	typedef TextureInstance type;
    typedef TextureInstance instance_type;
	typedef Context context_type;
    static bool MHE_EXPORT load(type& res, const FilePath& name, const context_type* context);
};

}

#endif
