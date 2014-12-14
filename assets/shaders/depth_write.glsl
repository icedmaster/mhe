[vertex]

[uniform transform 0 perframe]
uniform transform
{
	mat4 vp;
};

[uniform permodel 1 permodel]
uniform permodel
{
	mat4 model;
	mat4 normal;
};

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec3 tng;
layout (location = 3) in vec2 tex;

void main()
{
	gl_Position = vp * model * vec4(pos, 1.0f);
}

[fragment]
void main()
{
}