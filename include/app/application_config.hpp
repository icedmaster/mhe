#ifndef __APPLICATION_CONFIG_HPP__
#define __APPLICATION_CONFIG_HPP__

#include "core/string.hpp"
#include "core/types.hpp"
#include "core/compiler.hpp"
#include "application_asset_path.hpp"

namespace mhe {
namespace app {

static const char* default_config_filename = "mhe.config";

struct ApplicationConfig
{
    uint width;
    uint height;
    uint bpp;
    bool fullscreen;
    string assets_path;

    string render_config_filename;

    string default_shader_name;
    string default_texture_name;

    ApplicationConfig() :
        assets_path(default_assets_path())
    {}
};

MHE_EXPORT bool load_config(ApplicationConfig& config, const char* filename = default_config_filename);

}}

#endif
