#ifndef __SHADER_UTILS_HPP__
#define __SHADER_UTILS_HPP__

#include "utils/types_cast.hpp"

namespace mhe {

// default names of shader parameters
const std::string projection_matrix_uniform_name = "projection_matrix";
const std::string texture_unit_uniform_name = "texture_unit";
const std::string vertex_texcoord_attribute_name = "texcoord";
const std::string vertex_position_attribute_name = "position";
const std::string vertex_normal_attribute_name = "normals";
const std::string vertex_color_attribute_name = "color";

namespace utils {

inline std::string name_for_texture_unit(size_t unit)
{
	return texture_unit_uniform_name + utils::types_cast<std::string>(unit);
}

inline std::string name_for_texture_attribute(size_t unit)
{
	return vertex_texcoord_attribute_name + utils::types_cast<std::string>(unit);
}

}}

#endif
