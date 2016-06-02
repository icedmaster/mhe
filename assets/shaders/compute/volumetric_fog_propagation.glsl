[compute]

[include "../common.h"]

layout(rgba16f, binding = 0) readonly uniform image3D input_texture;
layout(rgba16f, binding = 1) writeonly uniform image3D output_texture;

layout(binding = 1) uniform Settings
{
    vec4 volume_size;
    vec4 fog_color;
};

[var THREADS_NUMBER 4]

vec4 scattering(vec4 current, ivec3 cell_coord)
{
    vec4 next_value = imageLoad(input_texture, cell_coord);
    float scattering_coeff = current.a;
    vec3 color = current.rgb + saturate(exp(-scattering_coeff)) * next_value.rgb;
    return vec4(color, scattering_coeff + next_value.a); // out scattering
}

layout(local_size_x = THREADS_NUMBER, local_size_y = THREADS_NUMBER) in;
void main()
{
    int depth = int(volume_size.z);
    ivec3 current_coord = ivec3(gl_GlobalInvocationID.xy, 0);
    vec4 current_color = imageLoad(input_texture, current_coord);
    for (int i = 1; i < depth; ++i)
    {
        ivec3 next_coord = ivec3(gl_GlobalInvocationID.xy, i);   
        vec4 next = scattering(current_color, next_coord);
        imageStore(output_texture, next_coord, vec4(next.rgb, exp(-next.a)));
        current_color = next; // accumulate
    }
}
