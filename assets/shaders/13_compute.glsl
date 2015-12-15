[compute]

layout(rgba8, binding = 0) readonly uniform image2D src;
layout(binding = 1) writeonly uniform image2D dst;

#define BLUR_KERNEL_SIZE 9

layout(local_size_x = 16, local_size_y = 16) in;
void main()
{
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	vec4 clr = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int x = -BLUR_KERNEL_SIZE; x <= BLUR_KERNEL_SIZE; ++x)
	{
		for (int y = -BLUR_KERNEL_SIZE; y <= BLUR_KERNEL_SIZE; ++y)
		{
			ivec2 offset = ivec2(x, y);
			vec4 src_color = imageLoad(src, pos + offset);
			clr += src_color;
		}
	}
	imageStore(dst, pos, clr / ((BLUR_KERNEL_SIZE * 2 + 1) * (BLUR_KERNEL_SIZE * 2 + 1)));
}