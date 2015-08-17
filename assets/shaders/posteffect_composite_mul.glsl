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

[include "common.h"]

[sampler2D texture0 0]
[sampler2D texture1 1]

in VSOutput vsoutput;

out vec4 color;

void main()
{
	color = texture(texture0, vsoutput.tex) * texture(texture1, vsoutput.tex);
}
