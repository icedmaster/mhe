[compute]

[include "../common.h"]
[include "../geometry_common.h"]
[include "../lighting_common.h"]

[var THREADS_NUMBER 4]

layout(rgba8, binding = 0) readonly uniform image2D noise_texture;
layout(binding = 1) uniform sampler2D shadow_texture;
layout(rgba8, binding = 2) writeonly uniform image3D output_texture;

layout(binding = 1) uniform Settings
{
    vec4 volume_size;
    vec4 fog_color;
};

layout(binding = 2) uniform LightData
{
    Light light;
};

float calculate_density(vec3 pos_ws)
{
    return 0.5f;
}

vec3 invocation_id_to_01(uvec3 pos)
{
    return vec3(pos.x / volume_size.x,
                pos.y / volume_size.y,
                pos.z / volume_size.z);
}

layout(local_size_x = THREADS_NUMBER, local_size_y = THREADS_NUMBER, local_size_z = THREADS_NUMBER) in;
void main()
{
    uvec3 volume_pos = gl_GlobalInvocationID.xyz;
    vec3 pos_ndc = invocation_id_to_01(volume_pos);
    vec3 pos_ws = position_from_depth(pos_ndc.xy, pos_ndc.z, inv_vp);
    
    float fog_density = calculate_density(pos_ws);
    // lighting
    float shadow = texture(shadow_texture, pos_ndc.xy).x;
    vec3 directional_light_radiance = light.diffuse.rgb * shadow;

    vec4 output_color = vec4((directional_light_radiance + fog_color.rgb) * fog_density, fog_density);

    imageStore(output_texture, ivec3(volume_pos), output_color);
}
