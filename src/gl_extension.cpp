#include "gl_extension.hpp"
#include <iostream>
#include <set>

#ifdef WIN32
PFNGLMULTITEXCOORD2FPROC glMultiTexCoord2f;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLCLIENTACTIVETEXTUREPROC glClientActiveTexture;

PFNGLCOMPILESHADERPROC glCompileShader;
#endif

namespace
{
    std::map<std::string, void*> ext_;
}

namespace mhe
{
    void load_default_extensions()
    {
        #ifdef WIN32
        glActiveTexture = (PFNGLACTIVETEXTUREPROC)SDL_GL_GetProcAddress("glActiveTexture");
        if (glActiveTexture == 0)
            std::cout << "Can't find glActiveTexture() extension\n";
        ext_["glActiveTexture"] = reinterpret_cast<void*>(glActiveTexture);
        glClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)SDL_GL_GetProcAddress("glClientActiveTexture");
        if (glClientActiveTexture == 0)
            std::cout << "Can't find glClientActiveTexture() extension\n";
        ext_["glClientActiveTexture"] = reinterpret_cast<void*>(glClientActiveTexture);
        glMultiTexCoord2f = (PFNGLMULTITEXCOORD2FPROC)SDL_GL_GetProcAddress("glMultiTexCoord2f");
        if (glMultiTexCoord2f == 0)
            std::cout << "Can't find glMultiTexCoord2f() extension\n";
        ext_["glMultiTexCoord2f"] = reinterpret_cast<void*>(glMultiTexCoord2f);

        // shaders
        glCompileShader = (PFNGLCOMPILESHADERPROC)SDL_GL_GetProcAddress("glCompileShader");
        if (glCompileShader == 0)
            std::cout << "Can't find glCompileShader() extension\n";
        ext_["glCompileShader"] = reinterpret_cast<void*>(glCompileShader);
        #endif
    }

    void* load_extension(const std::string& ext)
    {
        std::map<std::string, void*>::iterator it = ext_.find(ext);
        if (it != ext_.end())
            return it->second;
        void* addr = SDL_GL_GetProcAddress(ext.c_str());
        ext_[ext] = addr;
        return addr;
    }

    bool is_extension_present(const std::string& ext)
    {
        std::map<std::string, void*>::iterator it = ext_.find(ext);
        if ( (it != ext_.end()) && (it->second != 0) )
            return true;
        return false;
    }

	std::string get_availible_extensions()
	{
		return std::string(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
	}

	bool is_extension_availible(const char* ext)
	{
		static std::set<std::string> ext_present;
		if (ext_present.empty())
		{
			std::string e = get_availible_extensions();
			size_t pos = std::string::npos;
			do
			{
				pos = e.find_first_of(' ');
				std::string s(e);
				if (pos != std::string::npos)
				{
					s = e.substr(0, pos);
					e.erase(0, pos + 1);
				}
				ext_present.insert(s);
			}
			while (pos != std::string::npos);
		}

		std::set<std::string>::iterator it = ext_present.find(std::string(ext));
		if (it != ext_present.end())
			return true;
		return false;
	}
};
