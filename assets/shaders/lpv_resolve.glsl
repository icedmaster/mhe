#define EARLY_EXIT

[include "posteffect_vs_common.h"]

[fragment]

[include "sh.h"]

in VSOutput vsoutput;

[uniform InjectionSettings 1 permodel]
uniform InjectionSettings
{
    vec4 injection_settings;
    mat4 rsm_to_world;
    mat4 world_to_lpv;
    vec4 light_settings;
};

[sampler2D scene_normal_texture 0]
[sampler2D scene_depth_texture 1]
[sampler3D sh_r_texture 2]
[sampler3D sh_g_texture 3]
[sampler3D sh_b_texture 4]

out vec3 out_color;

void main()
{
    vec3 normal_ws = texture(scene_normal_texture, vsoutput.tex).xyz;
    float depth = texture(scene_depth_texture, vsoutput.tex).x;
#ifdef EARLY_EXIT
    if (depth > 0.9999f)
    {
        out_color = VEC3_ZERO;
        return;
    }
#endif
    vec3 position_ws = position_from_depth(vsoutput.tex, depth, inv_vp);
    vec3 position_lpv = (world_to_lpv * vec4(position_ws, 1.0f)).xyz;
    vec4 r = texture(sh_r_texture, position_lpv);
    vec4 g = texture(sh_g_texture, position_lpv);
    vec4 b = texture(sh_b_texture, position_lpv);
    RGBSH4 rgb_sh;
    rgb_sh.rgb[0] = r;
    rgb_sh.rgb[1] = g;
    rgb_sh.rgb[2] = b;
    out_color = calculate_irradiance(-normal_ws, rgb_sh);
}
