[defs PASS 0 1]
[defs QUALITY 0 2]

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

#if QUALITY == 0
#define BLUR_SAMPLES 1
#elif QUALITY == 1
#define BLUR_SAMPLES 3
#elif QUALITY == 2
#define BLUR_SAMPLES 5
#endif

[uniform blur_params 1 per_frame]
uniform blur_params
{
	vec4 params;
};

[include "common.h"]
[include "gaussian_blur_kernels.h"]

[sampler2D main_texture 0]

in VSOutput vsoutput;

out vec4 color;

#pragma optionNV (unroll all)
vec4 blur(vec2 tex, vec2 dir)
{
	vec4 res = VEC4_ZERO;
	vec2 inv_texsize = 1.0f / textureSize(main_texture, 0);
	for (int i = -BLUR_SAMPLES; i <= BLUR_SAMPLES; ++i)
	{
		vec4 c = texture(main_texture, tex + dir * i * inv_texsize) * blur_weights[i + BLUR_SAMPLES];
		res += c;
	}
	return res;
}

void main()
{
	float size = params.x;
#if PASS == 0
	vec4 res = blur(vsoutput.tex, vec2(size, 0));
#elif PASS == 1
	vec4 res = blur(vsoutput.tex, vec2(0, size));
#endif
	color = res;
}
