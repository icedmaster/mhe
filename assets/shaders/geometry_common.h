[uniform transform 0 perframe]
uniform transform
{
    mat4 vp;
    mat4 inv_vp;
    mat4 inv_proj;
    vec4 viewpos;
};

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
