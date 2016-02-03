[compute]

layout(std430, binding = 0) buffer InputData
{
	float input_data[];
};

layout(std430, binding = 1) buffer OutputData
{
	float output_data[];
};

#define THREADS_NUMBER 32

shared float workgroup_data[THREADS_NUMBER];

layout(local_size_x = THREADS_NUMBER) in;
void main()
{
	uint global_index = gl_WorkGroupID.x * gl_WorkGroupSize.x + gl_LocalInvocationID.x;
	workgroup_data[gl_LocalInvocationID.x] = input_data[global_index];
	barrier(); // data now is visible in other invocations

	if (gl_LocalInvocationID.x == 0)
	{
        memoryBarrierShared(); // wait until all threads reach this point of execution
		float average_luminance = 0.0f;
		for (int i = 0; i < THREADS_NUMBER; ++i)
			average_luminance += workgroup_data[i];
		average_luminance /= THREADS_NUMBER;
		output_data[gl_WorkGroupID.x] = average_luminance;
	}
}
