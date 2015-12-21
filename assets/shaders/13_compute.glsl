[defs LUMINANCE 0 1]

[compute]

layout(rgba8, binding = 0) readonly uniform image2D src;

#if LUMINANCE == 1

layout(std430, binding = 0) buffer shared_luminance
{
	float result_luminance[];
};

#define LUM_VECTOR vec3(0.299, 0.587, 0.114)

float calculate_luminance(vec3 c)
{
	return dot(c, LUM_VECTOR);
}

shared float group_luminance[16 * 16];

layout(local_size_x = 16, local_size_y = 16) in;
void main()
{
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	vec3 src_color = imageLoad(src, pos).rgb;
	float luminance = calculate_luminance(src_color);
	uint index = gl_LocalInvocationID.y * gl_WorkGroupSize.x + gl_LocalInvocationID.x;
	group_luminance[index] = luminance;
	memoryBarrierShared(); // now this data will be visible in other invocations
	barrier(); // wait until all threads calculate the luminance
	
	if (index == 0)
	{
		float average_luminance = 0.0f;
		for (int i = 0; i < 16 * 16; ++i)
			average_luminance += group_luminance[i];
		average_luminance /= (16 * 16);

		uint global_index = gl_WorkGroupID.y * gl_NumWorkGroups.x + gl_WorkGroupID.x;
		result_luminance[global_index] = average_luminance;
	}
}

#else
layout(std430, binding = 0) buffer shared_luminance
{
	float result_luminance[];
};

layout(binding = 1) writeonly uniform image2D dst;

#define BLUR_KERNEL_SIZE 9

layout(local_size_x = 16, local_size_y = 16) in;
void main()
{
	uint global_index = gl_WorkGroupID.y * gl_WorkGroupSize.x + gl_WorkGroupSize.x;
	uint total_groups = gl_WorkGroupSize.x * gl_WorkGroupSize.y;
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);
	vec4 clr = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int x = -BLUR_KERNEL_SIZE; x <= BLUR_KERNEL_SIZE; ++x)
	{
		for (int y = -BLUR_KERNEL_SIZE; y <= BLUR_KERNEL_SIZE; ++y)
		{
			ivec2 offset = ivec2(x, y);
			vec4 src_color = imageLoad(src, pos + offset);
			if (dot(src_color.rgb, vec3(0.33f, 0.33f, 0.33f)) > result_luminance[global_index])
			    src_color *= 1.5f;
			clr += src_color;
		}
	}
	imageStore(dst, pos, clr / ((BLUR_KERNEL_SIZE * 2 + 1) * (BLUR_KERNEL_SIZE * 2 + 1)));
}
#endif