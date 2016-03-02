[defs DAMP 0 1]

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

bool is_outside(vec3 pos)
{
    return pos.x > 1.0f || pos.y > 1.0f || pos.z > 1.0f ||
           pos.x < 0.0f || pos.y < 0.0f || pos.z < 0.0f;
}

RGBSH4 load_sh(vec3 pos)
{
    RGBSH4 res;
    res.rgb[0] = texture(sh_r_texture, pos);
    res.rgb[1] = texture(sh_g_texture, pos);
    res.rgb[2] = texture(sh_b_texture, pos);
    return res;
}

vec4 SHCNormalize(in vec4 res)
{
    // extract direction
    float l = dot(res.gba, res.gba);
    res.gba /= max(0.05f, sqrt(l));
    res.r = 1.0;
    return res;
}

RGBSH4 shnormalize(RGBSH4 sh)
{
    RGBSH4 res;
    res.rgb[0] = SHCNormalize(sh.rgb[0]);
    res.rgb[1] = SHCNormalize(sh.rgb[1]);
    res.rgb[2] = SHCNormalize(sh.rgb[2]);
    return res;
}

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
    RGBSH4 rgb_sh = load_sh(position_lpv);

#if DAMP == 1
    float cell_size = injection_settings.y;
    vec3 shifted_pos = position_ws + normal_ws * cell_size * 0.2f;
    vec3 shifted_position_lpv = (world_to_lpv * vec4(shifted_pos, 1.0f)).xyz;
    RGBSH4 shifted_rgb_sh;
    if (is_outside(shifted_position_lpv))
        shifted_rgb_sh = empty_rgbsh4();
    else
        shifted_rgb_sh = load_sh(shifted_position_lpv);
    RGBSH4 diff_sh = sub(shifted_rgb_sh, rgb_sh);
    RGBSH4 rgb_sh_norm = shnormalize(rgb_sh);
    RGBSH4 diff_sh_norm = shnormalize(diff_sh);
    vec3 atten = vec3(saturate(dot(rgb_sh_norm.rgb[0], diff_sh_norm.rgb[0])),
                      saturate(dot(rgb_sh_norm.rgb[1], diff_sh_norm.rgb[1])),
                      saturate(dot(rgb_sh_norm.rgb[2], diff_sh_norm.rgb[2])));
    float demp = pow(max3(atten), 0.2f);
    rgb_sh.rgb[0] *= demp;
    rgb_sh.rgb[1] *= demp;
    rgb_sh.rgb[2] *= demp;
#endif

    vec3 irradiance = calculate_irradiance(-normal_ws, rgb_sh);
    out_color = irradiance;
}
