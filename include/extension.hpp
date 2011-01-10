#ifndef _EXTENSION_HPP_
#define _EXTENSION_HPP_

#include <string>

//#include <GL/glext.h>

namespace mhe
{
    std::string get_extensions();
    bool is_extension_supported(const char* ext);
    void load_extensions();

    // pointers to extension functions
};

#endif
