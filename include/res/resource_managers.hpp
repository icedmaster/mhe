#ifndef __RESOURCE_MANAGERS_HPP__
#define __RESOURCE_MANAGERS_HPP__

#include "resource_manager.hpp"
#include "mesh_loader.hpp"
#include "shader_program_loader.hpp"
#include "texture_loader.hpp"

namespace mhe {

typedef ResourceManager<MeshLoader> MeshManager;
typedef ResourceManager<ShaderProgramLoader> ShaderManager;
typedef ResourceManager<TextureLoader> TextureManager;

}

#endif
