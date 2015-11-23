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

