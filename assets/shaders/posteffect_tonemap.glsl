struct VSOutput
{
	vec2 tex;
};

[vertex]

#define FULLSCREEN_LAYOUT
[include "geometry_common.h"]

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;
	gl_Position = pos;
}

[fragment]

[sampler2D main_texture 0]

in VSOutput vsoutput;

out vec4 color;

vec3 tonemap_filmic(vec3 c)
{
	vec3 color = max(vec3(0.0f, 0.0f, 0.0f), c - 0.004f);
	color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f) + 0.06f);
	return pow(color, vec3(2.2f, 2.2f, 2.2f));
}

void main()
{
	vec4 src = texture(main_texture, vsoutput.tex);
	color = vec4(tonemap_filmic(src.rgb), src.a);
}