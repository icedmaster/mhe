[defs SKINNING 0 1]

[vertex]

[include "skinning_common.h"]

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 nrm;
layout (location = 2) in vec4 tng;
layout (location = 3) in vec2 tex;
#if SKINNING == 1
layout (location = 4) in ivec4 ids;
layout (location = 5) in vec4 weights;
#endif

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

void main()
{
#if SKINNING == 0
	vec4 p = vec4(pos, 1.0f);
#else
	mat4 skinning_transform;
	skinning(skinning_transform, skin_texture, ids, weights);
	vec4 p = skinning_transform * vec4(pos, 1.0f);
#endif
	gl_Position = vp * model * p;
}

[fragment]
void main()
{
}