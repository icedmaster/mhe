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
    if (is_outside(pos))
        return empty_rgbsh4();
    RGBSH4 res;
    res.r = texture(sh_r_texture, pos);
    res.g = texture(sh_g_texture, pos);
    res.b = texture(sh_b_texture, pos);
    return res;
}

// next two functions are taken from the NVIDIA's GI sample
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
    res.r = SHCNormalize(sh.r);
    res.g = SHCNormalize(sh.g);
    res.b = SHCNormalize(sh.b);
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
    vec3 atten = vec3(saturate(dot(rgb_sh_norm.r, diff_sh_norm.r)),
                      saturate(dot(rgb_sh_norm.g, diff_sh_norm.g)),
                      saturate(dot(rgb_sh_norm.b, diff_sh_norm.b)));
    float demp = pow(max3(atten), 0.2f);
    rgb_sh.r *= demp;
    rgb_sh.g *= demp;
    rgb_sh.b *= demp;
#endif

    vec3 irradiance = calculate_irradiance(-normal_ws, rgb_sh);
    out_color = irradiance;
}
