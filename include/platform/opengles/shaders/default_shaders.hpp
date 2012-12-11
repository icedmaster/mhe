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

const std::string projection_matrix_uniform_name = "projection_matrix";
const std::string texture_unit_uniform_name = "texture_unit";

}}

#endif
