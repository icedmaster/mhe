#ifndef CUSTOM_PERCAMERA_UNIFORM
[uniform percamera 0 perframe]
uniform percamera
{
    mat4 vp;
	mat4 view;
	mat4 proj;
    mat4 inv_vp;
    mat4 inv_proj;
    vec4 viewpos;
	vec4 ambient;
	float znear;
	float zfar;
	vec2 viewport; 
	vec2 inv_viewport;
};
#endif

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
layout (location = 2) in vec4 tng;
layout (location = 3) in vec2 tex;
#if SKINNING == 1
layout (location = 4) in ivec4 ids;
layout (location = 5) in vec4 weights;
#endif

#endif
