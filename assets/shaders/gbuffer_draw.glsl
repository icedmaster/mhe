struct VSOutput
{
	vec2 tex;
};

#define FULLSCREEN_LAYOUT
[include "geometry_common.h"]

[vertex]

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;
	gl_Position = pos;
}

[fragment]

[include "gbuffer_common.h"]

[sampler2D light_texture 4]

in VSOutput vsoutput;

out vec4 color;

void main()
{
	vec4 albedo = texture(albedo_texture, vsoutput.tex);
	vec4 light = texture(light_texture, vsoutput.tex);
	vec3 baked_irradiance = gbuffer_baked_light(vsoutput.tex);
	float depth = gbuffer_depth(vsoutput.tex);

	color = vec4(light.rgb, depth < 1.0f ? 1.0f : 0.0f);
}
