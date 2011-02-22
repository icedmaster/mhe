#ifndef _TEXTURE_MANAGER_HPP_
#define _TEXTURE_MANAGER_HPP_

#include "itexture.hpp"
#include "impl/system_factory.hpp"
#include "utils/file_utils.hpp"
#include "utils/image_loader.hpp"
#include <map>

namespace mhe
{
    class TextureManager
    {
        private:
            typedef std::map< std::string, boost::shared_ptr<iTexture> > texmap;

            texmap tm_;
            boost::shared_ptr<iTexture> fake_;
        public:
            TextureManager();

            const boost::shared_ptr<iTexture>& load(const std::string& filename);
    };
};

#endif
