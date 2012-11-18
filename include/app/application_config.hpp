#ifndef __APPLICATION_CONFIG_HPP__
#define __APPLICATION_CONFIG_HPP__

#include <string>
#include "types.hpp"

namespace mhe {
namespace app {

static const std::string default_config_filename = "mhe.config";

struct ApplicationConfig
{
	cmn::uint width;
	cmn::uint height;
	cmn::uint bpp;
	bool fullscreen;
	std::string assets_path;	
};

bool load_config(ApplicationConfig& config, const std::string& filename = default_config_filename);

}}

#endif
