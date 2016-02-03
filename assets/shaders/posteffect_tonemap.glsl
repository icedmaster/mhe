[version 430]

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

[include "color_common.h"]
[include "posteffect_shared.h"]

[sampler2D main_texture 0]
[sampler2D depth_texture 3]
[sampler2D bloom_texture 4]

in VSOutput vsoutput;

out vec4 out_color;

vec3 tonemap_filmic(vec3 c)
{
	vec3 color = max(vec3(0.0f, 0.0f, 0.0f), c - 0.004f);
	color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f) + 0.06f);
	return pow(color, vec3(2.2f, 2.2f, 2.2f));
}

vec3 tonemap(vec3 c, float key, float average_luminance)
{
    vec3 color = exposed_color(c, key, average_luminance, 0.0f);
	vec3 tonemapped_color = tonemap_filmic(color) + texture(bloom_texture, vsoutput.tex).rgb;
    return tonemapped_color;
}

void main()
{
	float depth = texture(depth_texture, vsoutput.tex).r;
	vec4 src = texture(main_texture, vsoutput.tex);

    float key, threshold, intensity;
    unpack_bloom_settings(key, threshold, intensity);
	out_color = vec4(tonemap(src.rgb, key, average_luminance), depth < 1.0f ? src.a : 0.0f);
}
