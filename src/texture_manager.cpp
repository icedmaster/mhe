#include "texture_manager.hpp"
#include "bmp_image.hpp"
#include "png_image.hpp"
#include "siwa_image.hpp"

namespace mhe
{
    bool TextureManager::have_texture(const std::string& name) const
    {
        texmap::const_iterator it = tm_.find(name);
        if (it == tm_.end())
            return false;
        return true;
    }

    boost::shared_ptr<iTexture> TextureManager::load_impl(const std::string& name,
                                                          const std::string& sname)
    {
        Image* im = load_image(name);
        if (!im)
        {
            utils::global_log::instance().write("can't load image:" + name);
            return boost::shared_ptr<iTexture>();
        }

        utils::global_log::instance().printf("image %s loaded:%d %d", name.c_str(),
                                                                      im->width(),
                                                                      im->height());
        boost::shared_ptr<Image> image(im);
        boost::shared_ptr<iTexture> tex(SystemFactory::instance().createTexture());
        tex->setImage(image);
        tm_[sname] = tex;
        return tex;
    }

    void TextureManager::load(const std::string& name)
    {
        // get short name
        const std::string& fn = get_file_name(name);
        if (have_texture(fn)) return;

        load_impl(name, fn);
    }

    void TextureManager::unload(const std::string& name)
    {
        texmap::iterator it = tm_.find(name);
        if (it == tm_.end()) return;
        tm_.erase(it);
    }

    boost::shared_ptr<iTexture> TextureManager::get(const std::string& name)
    {
        const std::string& s_name = get_file_name(name);
        texmap::const_iterator it = tm_.find(s_name);
        if (it == tm_.end())
        {
            // try to load
            return load_impl(name, s_name);
        }
        return it->second;
    }
	
	Image* TextureManager::load_image(const std::string& filename) const
	{
	    const std::string& ext = get_file_extensiton(filename);
        if (ext.empty()) return nullptr;

        Image* im = nullptr;
        if (ext == "bmp")
        {
            im = new bmp_image;
            if (im->load(filename))
                return im;
        }
        else if (ext == "png")
        {
            im = new png_image;
            if (im->load(filename))
                return im;
        }
        else if (ext == "siwa")
        {
            im = new siwa_image;
            if (im->load(filename))
                return im;
        }
        if (im != nullptr)
            delete im;
        return nullptr;
	}
}
