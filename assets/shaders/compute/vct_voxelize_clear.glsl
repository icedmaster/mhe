[compute]

layout (r32ui, binding = 0) writeonly uniform uimage3D color_image;
layout (r32ui, binding = 1) writeonly uniform uimage3D normal_image;

[var THREADS_NUMBER 4]

layout (local_size_x = THREADS_NUMBER, local_size_y = THREADS_NUMBER, local_size_z = THREADS_NUMBER) in;
void main()
{
    uvec3 volume_pos = gl_GlobalInvocationID.xyz;
    imageStore(color_image, ivec3(volume_pos), uvec4(0));
    imageStore(normal_image, ivec3(volume_pos), uvec4(0));
}
