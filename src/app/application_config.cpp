#include "app/application_config.hpp"

#include <fstream>
#include "utils/options_parser.hpp"
#include "utils/global_log.hpp"
#include "utils/file_utils.hpp"

namespace mhe {
namespace app {

bool load_config(ApplicationConfig& config, const std::string& filename)
{
	std::ifstream f(filename.c_str());
	if (!f.is_open())
	{
		WARN_LOG("Can't open config with filename:" << filename);
		return false;
	}

	const std::string& str = utils::read_whole_file(f);
	f.close();
	if (str.empty())
	{
		WARN_LOG("Config file is empty:" << filename);
		return false;
	}

	utils::OptionsParser op(str, '\n');
	config.width = op.get<int>("width");
	config.height = op.get<int>("height");
	config.bpp = op.get<int>("bpp");
	config.fullscreen = op.get<bool>("fullscreen");
	config.assets_path = op.get<std::string>("assets_path");

	return true;
}

}}
