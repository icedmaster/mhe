[include "posteffect_vs_common.h"]

[fragment]

[include "common.h"]

[sampler2D depth_texture 3]
[sampler3D fog_texture 4]

in VSOutput vsoutput;

out vec4 out_color;

void main()
{
    float depth = texture(depth_texture, vsoutput.tex).x;
    float linear_depth = saturate(linearized_depth(depth, znear, zfar) / zfar);
    vec4 fog = texture(fog_texture, vec3(vsoutput.tex, linear_depth));
    out_color = vec4(fog.rgb, 1.0f - fog.a);
}
