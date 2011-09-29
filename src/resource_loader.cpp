#include "resource_loader.hpp"

namespace mhe {

iSound* SoundLoader::load(const std::string& name, boost::shared_ptr<helper_type> /*helper*/)
{
	const std::string& ext = get_file_extension(name);
	boost::shared_ptr<iSoundData> data;
	bool loaded = false;
	if (ext == "ogg")
	{
		data.reset(new ogg_sound);
		if (data->load(name))
			loaded = true;
	}
	else if (ext == "wav")
	{
		data.reset(new wav_sound);
		if (data->load(name))
			loaded = true;
	}
	if (loaded)
	{
		iSound* sound = SystemFactory::instance().createSound();
		if (sound->init(data))
			return sound;
		delete sound;
	}
	return nullptr;
}

iTexture* TextureLoader::load(const std::string& filename, boost::shared_ptr<helper_type> helper)
{
	const std::string& ext = get_file_extension(filename);
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
		iTexture* texture = SystemFactory::instance().createTexture();
		texture->setImage(im, helper);
		return texture;
	}
	return nullptr;
}

gui::iFont* FontLoader::load(const std::string& name, boost::shared_ptr<helper_type> /*helper*/)
{
	const std::string& ext = get_file_extension(name);
	if (ext.empty()) return 0;

	if (ext == "fnt")
	{
		gui::BMFont* font = new gui::BMFont;
		if (!font->load(name))
		{
			delete font;
			return nullptr;
		}
		return font;
	}

	return 0;
}

}	// mhe
