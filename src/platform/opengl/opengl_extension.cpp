#include "platform/opengl/opengl_extension.hpp"
#include "utils/logutils.hpp"
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace mhe {
namespace opengl {
void OpenGLExtensions::init_extensions()
{
#ifndef MHE_OPENGLES
	// multitexture
	glActiveTexture_ = load_extension<PFNGLACTIVETEXTUREARBPROC>("glActiveTexture");
	glClientActiveTexture_ = load_extension<PFNGLCLIENTACTIVETEXTUREARBPROC>("glClientActiveTexture");
	glMultiTexCoord2f_ = load_extension<PFNGLMULTITEXCOORD2FARBPROC>("glMultiTexCoord2f");

	get_str_extensions();

	INFO_LOG("supported extensions:");
	for (std::map<std::string, bool>::iterator it = loaded_extensions_.begin();
		 it != loaded_extensions_.end(); ++it)
	{
		if (it->second)
			INFO_LOG(it->first);
	}
#endif
}

bool OpenGLExtensions::is_extension_supported(const std::string& ext_name) const
{
#ifndef MHE_OPENGLES
	std::map<std::string, bool>::const_iterator it = loaded_extensions_.find(ext_name);
	if (it == loaded_extensions_.end()) return false;
	return it->second;
#else
    UNUSED(ext_name);
  	return true;
#endif
}

std::string OpenGLExtensions::get_supported_extensions() const
{
	const char* ext = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
	if (ext == nullptr) return std::string();
	return std::string(ext);
}

void OpenGLExtensions::get_str_extensions()
{
	const std::string& strext = get_supported_extensions();
	std::vector<std::string> ext;

	boost::split(ext, strext,
				 boost::is_any_of(" "), boost::token_compress_on);
	for (size_t i = 0; i < ext.size(); ++i)
		loaded_extensions_[ext[i]] = true;
}

}}	// mhe
