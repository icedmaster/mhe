#ifndef __APPLICATION_ASSET_PATH_HPP__
#define __APPLICATION_ASSET_PATH_HPP__

#include <core/string.hpp>

namespace mhe {
namespace app {

inline string default_assets_path()
{
#ifdef MHE_STANDALONE
    return "assets/";
#elif defined MHE_VS
    return "../../assets/";
#else
    return "../assets/";
#endif
}

const mhe::string assets_base_path = "assets/";
const mhe::string texture_path = "textures/";
const mhe::string font_path = "fonts/";
const mhe::string shader_path = "shaders/";
const mhe::string mesh_path = "meshes/";
const mhe::string material_path = "materials/";
const mhe::string objects_path = "objects/";

inline string default_objects_path()
{
    return default_assets_path() + objects_path;
}

}}

#endif
