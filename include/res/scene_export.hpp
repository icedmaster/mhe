#ifndef __SCENE_EXPORT_HPP__
#define __SCENE_EXPORT_HPP__

#include "core/types.hpp"
#include "core/string.hpp"

namespace mhe {

struct MeshPartExportData
{
	uint32_t vbuffer_offset;
	uint32_t ibuffer_offset;
	uint32_t faces_number;
	uint32_t material_index;
};

struct TextureExportData
{
	FilePath name;
	uint8_t mode;
};

struct MaterialExportData
{
	TextureExportData albedo_texture;
	TextureExportData normalmap_texture;
};

}

#endif