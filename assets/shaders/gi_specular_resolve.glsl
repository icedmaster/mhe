[version 430]

[include "posteffect_vs_common.h"]

[fragment]

[include "common.h"]
[include "geometry_common.h"]
[include "gbuffer_pbr_common.h"]

in VSOutput vsoutput;

[samplerCube cubemap 4]

out vec3 out_color;

void main()
{
    float depth = gbuffer_depth(vsoutput.tex);
    if (depth == 1.0f)
    {
        out_color = VEC3_ZERO;
        return;
    }
    vec3 pos_ws = position_from_depth(vsoutput.tex, gbuffer_depth(vsoutput.tex), inv_vp);
    vec3 viewdir = normalize(pos_ws - viewpos.xyz);

    vec3 normal = gbuffer_normal_ws(vsoutput.tex);
    vec3 reflected = reflect(viewdir, normal);

    float roughness = gbuffer_roughness(vsoutput.tex);
    float lod = roughness * log2(textureSize(cubemap, 0).x);

    out_color = textureLod(cubemap, reflected, lod).rgb;
}
