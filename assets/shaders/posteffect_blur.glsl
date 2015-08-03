[defs PASS 0 1]
[defs QUALITY 0 1]

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
#define BLUR_SAMPLES 3
#elif QUALITY == 1
#define BLUR_SAMPLES 3
#endif

[include "common.h"]
[include "gaussian_blur_kernels.h"]

[sampler2D main_texture 0]

in VSOutput vsoutput;

out vec4 color;

#pragma optionNV (unroll all)
vec4 blur(vec2 tex, ivec2 dir)
{
	vec4 res = VEC4_ZERO;
	/*for (int i = -BLUR_SAMPLES; i <= BLUR_SAMPLES; ++i)
	{
		vec4 c = textureOffset(main_texture, tex, dir * i) * blur_weights[i + BLUR_SAMPLES];
		res += c;
	}*/
	return res;
}

void main()
{
#if PASS == 0
	vec4 res = blur(vsoutput.tex, ivec2(1, 0));
#elif PASS == 1
	vec4 res = blur(vsoutput.tex, ivec2(0, 1));
#endif
	color = res;
}
