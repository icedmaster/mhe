[compute]

#define LIGHT_TYPE DIRECTIONAL_LIGHT
#define CASCADED_SHADOWMAP

[include "../common.h"]
[include "../geometry_common.h"]
[include "../lighting_common.h"]

[var THREADS_NUMBER 4]

layout(rgba16f, binding = 0) readonly uniform image2D noise_texture;
layout(binding = 1) uniform sampler2D shadowmap_texture;
layout(rgba16f, binding = 2) writeonly uniform image3D output_texture;

layout(binding = 1) uniform Settings
{
    vec4 volume_size;
    vec4 fog_color;
    vec4 fog_settings;
};

layout(binding = 2) uniform LightData
{
    Light light;
};

layout(binding = 3) uniform ESMSettings
{
    vec4 esm_settings;
};

float calculate_shadow_esm(vec3 pos_ws, float linear_depth)
{
    float exponent = esm_settings.x;

    int cascade = calculate_cascade(linear_depth, light);
    
    vec4 pos_ls = light.lightvp[cascade] * vec4(pos_ws, 1.0f);
    vec3 pos_hcs = pos_ls.xyz * light.csm_scale[cascade].xyz + light.csm_offset[cascade].xyz;

    float cascades_number = light.cascades_number;
    vec2 scale = vec2(1.0f / cascades_number, 1.0f);
    vec2 offset = vec2(cascade * scale.x, 0.0f);

	float receiver = max(exp((pos_hcs.z * 0.5f + 0.5f) * exponent), 0.001f);
    float occluder = texture(shadowmap_texture, (pos_hcs.xy * 0.5f + 0.5f) * scale + offset).x;

    return saturate(occluder / receiver); 
}

float calculate_density(vec3 pos_ws)
{
    float density = fog_settings.x;
    float falloff = fog_settings.y;
	float scattering_coeff = fog_settings.z;
    vec3 viewdir = pos_ws - viewpos.xyz;
    float dist = length(viewdir);
    viewdir /= dist;
	float ry = abs(viewdir.y) < 0.001f ? 0.001f : viewdir.y;
    float height_based_amount = density * saturate(exp(-viewpos.y * falloff) * (1.0f - exp(-dist * ry * falloff)) / (falloff * ry));
    return height_based_amount * scattering_coeff;
}

vec3 invocation_id_to_01(uvec3 pos)
{
    return vec3(pos.x / volume_size.x,
                pos.y / volume_size.y,
                pos.z / volume_size.z);
}

vec3 ldr_light(vec3 hdr_light_value)
{
	return hdr_light_value / (hdr_light_value + vec3(1.0f));
}

layout(local_size_x = THREADS_NUMBER, local_size_y = THREADS_NUMBER, local_size_z = THREADS_NUMBER) in;
void main()
{
    uvec3 volume_pos = gl_GlobalInvocationID.xyz;
    vec3 pos_ndc = invocation_id_to_01(volume_pos);
    vec3 viewray = (inv_vp * vec4(pos_ndc.xy * 2.0f - 1.0f, 1.0f, 1.0f)).xyz;
    float linear_depth = pos_ndc.z * zfar;
    vec3 pos_ws = viewpos.xyz + viewray * linear_depth;
    
	float fog_density = calculate_density(pos_ws);
    // lighting
vec3 directional_light_radiance = light.diffuse.rgb * calculate_shadow_esm(pos_ws, linear_depth);

	vec4 output_color = vec4((directional_light_radiance + ambient.rgb) * fog_color.rgb * fog_density, fog_density);

    imageStore(output_texture, ivec3(volume_pos), output_color);
}
