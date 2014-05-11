#ifndef __APPLICATION_CONFIG_HPP__
#define __APPLICATION_CONFIG_HPP__

#include <string>
#include "core/types.hpp"

namespace mhe {
namespace app {

static const std::string default_config_filename = "mhe.config";

struct ApplicationConfig
{
	uint width;
	uint height;
	uint bpp;
	bool fullscreen;
	std::string assets_path;

	std::string default_shader_name;
	std::string default_texture_name;
};

bool load_config(ApplicationConfig& config, const std::string& filename = default_config_filename);

}}

#endif
