[version 430]

[vertex]

#define CUSTOM_PERCAMERA_UNIFORM
[include "geometry_common.h"]

struct VSOutput
{
	vec3 pos;
	vec3 nrm;
};

out VSOutput vsoutput;

void main()
{
	vsoutput.pos = (model * vec4(pos, 1.0f)).xyz;
	vsoutput.nrm = (normal * vec4(nrm, 0.0f)).xyz;
}

[geometry]

in VSOutput vsoutput[3];
out VSOutput gsoutput;

layout (triangles) in;
layout (triangles) out;

void main()
{
    for (int i = 0; i < gl_in.length(); ++i)
    {
        gsoutput = vsoutput[i];       
		EmitVertex();
    }
	EndPrimitive();
}

[fragment]

VSOutput gsoutput;

void main()
{
}
