#ifndef __SCENE_EXPORT_HPP__
#define __SCENE_EXPORT_HPP__

#include "core/types.hpp"
#include "core/string.hpp"
#include "math/vector3.hpp"
#include "math/aabb.hpp"
#include "render/material.hpp" // MaterialRenderData

namespace mhe {

struct MeshPartExportData
{
	uint32_t vbuffer_offset;
	uint32_t ibuffer_offset;
	uint32_t faces_number;
	uint32_t material_index;
	AABBf aabb;
};

struct TextureExportData
{
	FilePath name;
	uint8_t mode;
};

struct MaterialExportData
{
	FilePath name;
	string material_system;
	string lighting_model;
	MaterialRenderData data;
	TextureExportData albedo_texture;
	TextureExportData normalmap_texture;
};

struct MeshExportData
{
	AABBf aabb;
};

}

#endif
