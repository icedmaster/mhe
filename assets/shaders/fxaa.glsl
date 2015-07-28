[defs QUALITY 0 2]

#ifndef QUALITY
#define QUALITY 1
#endif

#define FXAA_PC 1
#define FXAA_GREEN_AS_LUMA 1
#define FXAA_GLSL_130 1

#if (QUALITY == 0)
#define FXAA_QUALITY__PRESET 12
#endif

#if (QUALITY == 1)
#define FXAA_QUALITY__PRESET 23
#endif

#if (QUALITY == 2)
#define FXAA_QUALITY__PRESET 39
#endif

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

#ifdef GL_ARB_gpu_shader5
#extension GL_ARB_gpu_shader5 : enable
#endif

[include "Fxaa3_11.h"]

[sampler2D main_texture 0]

in VSOutput vsoutput;

out vec4 color;

void main()
{
	float fxaaSubpix = 0.75;
    float fxaaEdgeThreshold = 0.166;
    float fxaaEdgeThresholdMin = 0.0833;	
	vec4 vec4_zero = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	vec4 res = FxaaPixelShader(vsoutput.tex, vec4_zero, main_texture, main_texture, main_texture, inv_viewport, vec4_zero, vec4_zero, vec4_zero, fxaaSubpix, fxaaEdgeThreshold, fxaaEdgeThresholdMin, 0, 0, 0, vec4_zero); 
	color = vec4(res.rgb, 1.0f);
}
