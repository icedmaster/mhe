[include "posteffect_vs_common.h"]

[fragment]

[include "common.h"]

in VSOutput vsoutput;

[uniform FogSettings 1 perframe]
uniform FogSettings
{
    vec4 start_and_density;
    vec4 fog_color;
};

[sampler2D depth_texture 3]

out vec4 out_color;

void main()
{
    float start = start_and_density.x;
    float density = start_and_density.y;
    float falloff = start_and_density.z;

    float depth = texture(depth_texture, vsoutput.tex).x;
    vec3 pos_ws = position_from_depth(vsoutput.tex, depth, inv_vp);
    vec3 ray_dir = pos_ws - viewpos.xyz;
    float dist = length(ray_dir);
    ray_dir /= dist;

    float ry = ray_dir.y;
    float height_based_amount = density * saturate(exp(-viewpos.y * falloff) * (1.0f - exp(-dist * ry * falloff)) / (falloff * ry));

    out_color = vec4(fog_color.rgb, height_based_amount);
}
