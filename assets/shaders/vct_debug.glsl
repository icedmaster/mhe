[version 450]

[vertex]

#define DEFINE_BINDING_POINT_IN_SHADER
[include "geometry_common.h"]

[uniform Settings 1 percamera]
layout (binding = 1) uniform Settings
{
    vec4 grid_size;
    mat4 vct_vp[3];
    mat4 worldspace_to_voxelspace;
};

layout (r32ui, binding = 2) uniform readonly uimage3D color_image;

out flat uvec3 voxel_pos;
out vec4 voxel_color;

uvec3 uint_to_uvec3(uint v)
{
	uint x = v & 0x3ff;
	uint y = (v >> 10) & 0x3ff;
	uint z = (v >> 20) & 0x3ff;
    return uvec3(x, y, z);
}

vec4 uint_to_vec4(uint v)
{
	float x = float(v & 0xff) / 255.0f;
	float y = float((v >> 8) & 0xff) / 255.0f;
	float z = float((v >> 16) & 0xff) / 255.0f;
	float w = float((v >> 24) & 0xff) / 255.0f;
	return vec4(x, y, z, w);
}

void main()
{
	int voxel_index = gl_InstanceID;
    int grid_dim = int(grid_size.x);
    int x = voxel_index % grid_dim;
    int y = (voxel_index % (grid_dim * grid_dim)) / grid_dim;
    int z = voxel_index / (grid_dim * grid_dim);
    voxel_pos = uvec3(x, y, z);
    float cell_size = grid_size.w;
	mat4 world = mat4(cell_size, 0.0f, 0.0f, 0.0f,
					  0.0f, cell_size, 0.0f, 0.0f,
					  0.0f, 0.0f, cell_size, 0.0f,
					  (voxel_pos.x - grid_dim * 0.5f) * cell_size, (voxel_pos.y - grid_dim * 0.5f) * cell_size, (voxel_pos.z - grid_dim * 0.5f) * cell_size, 1.0f);
	uint color_ui = imageLoad(color_image, ivec3(voxel_pos)).x;
    voxel_color = uint_to_vec4(color_ui);
					  
	gl_Position = vp * world * vec4(pos, 1.0f);	
}

[fragment]

layout (r32ui, binding = 3) uniform readonly uimage3D normal_image; 

in flat uvec3 voxel_pos;
in vec4 voxel_color;
out vec4 out_color;

void main()
{
    if (voxel_color.a < 0.01f) discard;
	out_color = voxel_color;
}
