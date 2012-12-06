#ifndef __DEFAULT_SHADERS_HPP__
#define __DEFAULT_SHADERS_HPP__

namespace mhe {
namespace opengl {

static const char* default_vertex_shader_data = {
#include "default_vertex_shader.vert"
};

static const char* default_fragment_shader_data = {
#include "default_fragment_shader.frag"
};

static const std::string projection_matrix_uniform_name = "projection_matrix";
static const std::string vertex_position_attr_name = "position";
static const std::string vertex_color_attr_name = "color";
static const std::string texture_coord_attr_name = "texture_coord";
static const std::string normals_coord_attr_name = "normals";

}}

#endif
