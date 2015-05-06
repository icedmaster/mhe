[uniform percamera 0 perframe]
uniform percamera
{
    mat4 vp;
    mat4 inv_vp;
    mat4 inv_proj;
    vec4 viewpos;
	vec4 ambient;
	float znear;
	float zfar;
};

float linearized_depth(float d, float znear, float zfar)
{
	d = d * 2.0f - 1.0f;
	return (2 * znear * zfar) / (zfar + znear - d * (zfar - znear));
}

[vertex]

[uniform permodel 1 permodel]
uniform permodel
{
    mat4 model;
    mat4 normal;
};

#ifdef FULLSCREEN_LAYOUT
layout (location = 0) in vec4 pos;
layout (location = 1) in vec2 tex;
#elif defined(DEBUG_LAYOUT)
layout (location = 0) in vec3 pos;
#else
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec3 tng;
layout (location = 3) in vec2 tex;
#endif