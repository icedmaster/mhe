#include "texture_manager.hpp"

namespace mhe
{
    TextureManager::TextureManager() :
        fake_(SystemFactory::instance().createTexture())
    {
    }

    const boost::shared_ptr<iTexture>& TextureManager::load(const std::string& filename)
    {
        // get file name from full name
        const std::string& fn = get_file_name(filename);

        texmap::iterator it = tm_.find(fn);
        if (it != tm_.end())
            return it->second;
        // try to load image from file
        boost::shared_ptr<Image> im(load_image(filename));
        if (!im)
        {
            //global_log::instance().error("Can't load image from file " + filename);
            return fake_;
        }
        // image loaded, create texture
        boost::shared_ptr<iTexture> t(SystemFactory::instance().createTexture());
        t->setImage(im);
        std::pair <texmap::iterator, bool> ret_it = tm_.insert(texmap::value_type(fn, t));
        return ret_it.first->second;
    }
}
