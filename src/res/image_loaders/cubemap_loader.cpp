#include "res/image_loaders/cubemap_loader.hpp"

#include <cstring>
#include "render/image.hpp"
#include "pugixml/pugixml.hpp"
#include "utils/global_log.hpp"
#include "utils/file_utils.hpp"
#include "res/image_loaders/image_loaders_helper.hpp"

namespace mhe {

namespace detail {

bool load_cubemap(Image& image, pugi::xml_node node, const FilePath& filename)
{
	const char* nodes_names[6] = {"negx", "posx", "negy", "posy", "negz", "posz"};
	Image tmp_image;
    const FilePath& path = utils::get_file_path(filename);
	size_t stride = 0;
	for (int i = 0; i < 6; ++i)
	{
		pugi::xml_node n = node.child(nodes_names[i]);
		if (!n)
		{
			WARN_LOG("load_cubemap(): node " << nodes_names[i] << " not found");
			return false;
		}
        FilePath name = utils::path_join(path, FilePath(n.child_value()));
		if (!load_image_by_extension(tmp_image, name))
		{
			WARN_LOG("load_cubemap(): can't load file " << name);
			return false;
		}
		if (i == 0)
		{
			stride = tmp_image.width * tmp_image.height * (tmp_image.bpp / 8);
			image.data.resize(stride * 6);
			image.width = tmp_image.width;
			image.height = tmp_image.height;
			image.bpp = tmp_image.bpp;
			image.mode = tmp_image.mode;
		}
		else
		{
			if (tmp_image.width != image.width || tmp_image.height != image.height || tmp_image.bpp != image.bpp)
			{
				WARN_LOG("load_cubemap(): parameters mismatch " << image.width << " " << image.height << " " << image.bpp << " != " <<
						 tmp_image.width << " " << tmp_image.height << " " << tmp_image.bpp);
				return false;
			}
		}
		::memcpy(&image.data[stride * i], &tmp_image.data[0], stride);
	}
	return true;
}

}

bool load_cubemap(Image& image, std::istream& stream, const FilePath& filename)
{
	pugi::xml_document doc;
	if (!doc.load(stream))
	{
		WARN_LOG("load_cubemap() error: xml loading failed");
		return false;
	}
	pugi::xml_node root = doc.root();
	if (!root)
		return false;
	pugi::xml_node mhe_node = root.child("mhe");
	if (!mhe_node)
		return false;
	pugi::xml_node cubemap_node = mhe_node.child("cubemap");
	if (!cubemap_node)
	{
		WARN_LOG("load_cubemap(): can't find a cubemap node");
		return false;
	}
	return detail::load_cubemap(image, cubemap_node, filename);
}

}
