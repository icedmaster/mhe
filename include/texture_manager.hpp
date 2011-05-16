#ifndef _TEXTURE_MANAGER_HPP_
#define _TEXTURE_MANAGER_HPP_

#include "itexture.hpp"
#include "utils/file_utils.hpp"
#include "impl/system_factory.hpp"
#include "utils/logutils.hpp"
#include <map>

namespace mhe
{
    class TextureManager
    {
        private:
            typedef std::map< std::string, boost::shared_ptr<iTexture> > texmap;
            texmap tm_;
			
			Image* load_image(const std::string& filename) const;

            bool have_texture(const std::string& name) const;
            boost::shared_ptr<iTexture> load_impl(const std::string& name,
                                                  const std::string& sname);
        public:
            void load(const std::string& name);
            void unload(const std::string& name);

            // This functions is not 'const', because if texture with name <name>
            // not found, we try to load it
            boost::shared_ptr<iTexture> get(const std::string& name);
    };
}

#endif
