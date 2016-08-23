[include "posteffect_vs_common.h"]

[fragment]

#define LIGHT_TYPE DIRECTIONAL_LIGHT
#define CASCADED_SHADOWMAP

[include "common.h"]
[include "lighting_common.h"]

in VSOutput vsoutput;

[sampler2D esm_texture 0]
[sampler2D depth_texture 3]

[uniform Lights 1 permodel]
uniform Lights
{
    Light light;
};

[uniform ESMSettings 2 permodel]
uniform ESMSettings
{
    vec4 settings;
};

out float out_color;

void main()
{
    float exponent = settings.x;
    float depth = texture(depth_texture, vsoutput.tex).x;
    float linear_depth = linearized_depth(depth, znear, zfar);
    vec3 pos_ws = position_from_depth(vsoutput.tex, depth, inv_vp);
    
    int cascade = calculate_cascade(linear_depth, light);
    
    vec4 pos_ls = light.lightvp[cascade] * vec4(pos_ws, 1.0f);
    vec3 pos_hcs = pos_ls.xyz * light.csm_scale[cascade].xyz + light.csm_offset[cascade].xyz;

    float cascades_number = light.cascades_number;
    vec2 scale = vec2(1.0f / cascades_number, 1.0f);
    vec2 offset = vec2(cascade * scale.x, 0.0f);

    float receiver = exp((pos_hcs.z * 0.5f + 0.5f) * exponent);
    float occluder = texture(esm_texture, (pos_hcs.xy * 0.5f + 0.5f) * scale + offset).x;
    
    out_color = saturate(occluder / receiver);
} 
