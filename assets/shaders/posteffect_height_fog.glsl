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

    float depth = texture(depth_texture, vsoutput.tex).x;
    vec3 pos_vs = position_from_depth(vsoutput.tex, depth, inv_proj);
    float dist = abs(pos_vs.z);

    float distance_with_offset = max(dist - start, 0.0f);
    float amount = 1.0f - saturate(1.0f / exp(distance_with_offset * density));

    out_color = vec4(fog_color.rgb, amount);
}
