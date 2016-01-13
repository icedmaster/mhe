[include "posteffect_vs_common.h"]

[fragment]

[include "common.h"]
[include "gbuffer_common.h"]

in VSOutput vsoutput;

[samplerCube cubemap 4]

out vec4 color;

void main()
{
	GBuffer gbuffer = gbuffer_unpack(vsoutput.tex);
	vec3 pos_ws = position_from_depth(vsoutput.tex, gbuffer.depth, inv_vp);
	vec3 view_dir = normalize(pos_ws - viewpos.xyz);
	vec3 reflected_dir = reflect(view_dir, gbuffer.normal);
	color = vec4(texture(cubemap, reflected_dir).rgb * gbuffer.glossiness, 1.0f);
}
