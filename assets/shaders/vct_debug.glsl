[version 450]

[vertex]

#define DEFINE_BINDING_POINT_IN_SHADER
[include "geometry_common.h"]

layout (r32ui, binding = 1) uniform readonly uimageBuffer position_buffer; 

out flat uvec3 voxel_pos;

uvec3 uint_to_uvec3(uint v)
{
	uint x = v & 0x3ff;
	uint y = (v >> 10) & 0x3ff;
	uint z = (v >> 20) & 0x3ff;
    return uvec3(x, y, z);
}

void main()
{
	int voxel_index = gl_InstanceID;
	voxel_pos = uint_to_uvec3(imageLoad(position_buffer, voxel_index).x);
	mat4 world = mat4(1.0f, 0.0f, 0.0f, 0.0f,
					  0.0f, 1.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 1.0f, 0.0f,
					  voxel_pos.x, voxel_pos.y, voxel_pos.z, 1.0f);
					  
	gl_Position = vp * world * vec4(pos, 1.0f);	
}

[fragment]

layout (r32ui, binding = 2) uniform readonly uimage3D color_image;
layout (r32ui, binding = 3) uniform readonly uimage3D normal_image; 

in flat uvec3 voxel_pos;
out vec4 out_color;

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
	uint color_ui = imageLoad(color_image, ivec3(voxel_pos)).x;
	//out_color = uint_to_vec4(color_ui);
	out_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
