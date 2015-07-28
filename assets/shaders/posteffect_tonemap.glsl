[defs INPUTS 0 1]

#define INPUTS_NUMBER INPUTS + 1

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

[uniform tonemap_parameters 1 perframe]
uniform tonemap_parameters
{
	vec4 params;
};

#define GAMMA(p) p.x

[sampler2D main_texture 0]
[sampler2D ssr_texture 1]

in VSOutput vsoutput;

out vec4 color;

// TODO: Real tonemapping will be used after when I add HDR rendering
vec3 tonemap_filmic(vec3 c)
{
	c *= 2; // TODO: add exposure
	vec3 color = max(vec3(0.0f, 0.0f, 0.0f), c - 0.004f);
	color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f) + 0.06f);
	return pow(color, vec3(2.2f, 2.2f, 2.2f));
}

vec3 color_correction(vec3 c, float gamma)
{
	return pow(c, vec3(gamma, gamma, gamma));
}

void main()
{
	vec4 src = texture(main_texture, vsoutput.tex);
	color = vec4(color_correction(src.rgb, 0.9f), src.a);
#if INPUTS_NUMBER == 2
	vec4 refl = texture(ssr_texture, vsoutput.tex);
	color += vec4(refl.rgb, 0.0f);
#endif
}
