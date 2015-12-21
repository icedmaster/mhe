[compute]

layout(rgba8, binding = 0) readonly uniform image2D src;
layout(rgba8, binding = 1) writeonly uniform image2D dst;

layout(local_size_x = 16, local_size_y = 16) in;
void main()
{
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	vec4 c1 = imageLoad(src, pos * 4 + ivec2(-1, -1));
	vec4 c2 = imageLoad(src, pos * 4 + ivec2(-1, 1));
	vec4 c3 = imageLoad(src, pos * 4 + ivec2(1, 1));
	vec4 c4 = imageLoad(src, pos * 4 + ivec2(1, -1));
	imageStore(dst, pos, (c1 + c2 + c3 + c4) * 0.25f);
}