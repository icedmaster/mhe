#ifndef __TEXTURE_LOADER_HPP__
#define __TEXTURE_LOADER_HPP__

#include <string>
#include "core/compiler.hpp"

namespace mhe {

struct Context;
struct TextureInstance;

struct TextureLoader
{
	typedef TextureInstance type;
	typedef Context context_type;
	static bool MHE_EXPORT load(type& res, const std::string& name, const context_type* context);
};

}

#endif
