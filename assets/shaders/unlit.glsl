struct VSOutput
{
	vec2 tex;
};

[vertex]

[include "geometry_common.h"]

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;

	gl_Position = vp * model * vec4(pos, 1.0f);
}

[fragment]

[sampler2D texture 0]

in VSOutput vsoutput;

out vec4 color;

void main()
{
	color = texture2D(texture, vsoutput.tex);
}
