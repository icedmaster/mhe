[defs MODE 0 6]

#define MODE_RGBA 0
#define MODE_RGB 1
#define MODE_R 2
#define MODE_G 3
#define MODE_B 4
#define MODE_A 5
#define MODE_DEPTH 6

struct VSOutput
{
	vec2 tex;
};

[vertex]

layout (location = 0) in vec4 pos;
layout (location = 1) in vec2 tex;

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;
	gl_Position = pos;
}

[fragment]

[include "geometry_common.h"]
[include "common.h"]

[sampler2D main_texture 0]

in VSOutput vsoutput;

out vec4 color;

void main()
{
    vec4 data = texture(main_texture, vsoutput.tex);
#if MODE == MODE_RGBA
    color = data;
#elif MODE == MODE_RGB
    color = vec4(data.rgb, 1.0f);
#elif MODE == MODE_R
    color = vec4(data.rrr, 1.0f);
#elif MODE == MODE_G
    color = vec4(data.ggg, 1.0f);
#elif MODE == MODE_B
    color = vec4(data.bbb, 1.0f);
#elif MODE == MODE_A
    color = vec4(data.aaa, 1.0f);
#elif MODE == MODE_DEPTH
	float d = linearized_depth(data.x, znear, zfar) / zfar;
	color = vec4(d, d, d, 1.0f);
#endif
}
