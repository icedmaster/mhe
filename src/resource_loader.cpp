#include "resource_loader.hpp"

#include "mhe_sound.hpp"

namespace mhe {

iSound* SoundLoader::load(const std::string& name, const helper_type& /*helper*/)
{
	const std::string& ext = utils::get_file_extension(name);
	boost::shared_ptr<iSoundData> data;
	bool loaded = false;
	if (ext == "ogg")
	{
#ifdef MHE_HAS_VORBIS
		data.reset(new ogg_sound);
		if (data->load(name))
			loaded = true;
#endif
	}
	else if (ext == "wav")
	{
		data.reset(new wav_sound);
		if (data->load(name))
			loaded = true;
	}
	if (loaded)
	{
		iSound* sound = SystemFactory::instance().create_sound();
		if (sound->init(data))
			return sound;
		delete sound;
	}
	return nullptr;
}

Texture* TextureLoader::load(const std::string& filename, const helper_type& helper)
{
	const std::string& ext = utils::get_file_extension(filename);
	if (ext.empty()) return nullptr;

	boost::shared_ptr<Image> im;
	bool loaded = false;
	if (ext == "bmp")
	{
		im.reset(new bmp_image);
		if (im->load(filename))
			loaded = true;
	}
	else if (ext == "png")
	{
		im.reset(new png_image);
		if (im->load(filename))
			loaded = true;
	}
	else if (ext == "siwa")
	{
		im.reset(new siwa_image);
		if (im->load(filename))
			loaded = true;
	}
	if (loaded)
	{
		Texture* texture = SystemFactory::instance().create_texture();
		texture->set_image(im, helper);
		return texture;
	}
	return nullptr;
}

}	// mhe
