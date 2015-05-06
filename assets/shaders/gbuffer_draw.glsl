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

[sampler2D albedo_texture 0]
[sampler2D light_texture 1]

in VSOutput vsoutput;

out vec4 color;

void main()
{
	vec4 albedo = texture(albedo_texture, vsoutput.tex);
	vec4 light = texture(light_texture, vsoutput.tex);

	color = vec4(albedo.rgb * light.rgb + albedo.rgb * ambient.rgb, albedo.a > 0.0f ? 1.0f : 0.0f);
}