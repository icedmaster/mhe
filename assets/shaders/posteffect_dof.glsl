struct VSOutput
{
	vec2 tex;
};

#define FULLSCREEN_LAYOUT
[include "geometry_common.h"]

[vertex]

out VSOutput vsoutput;

void main()
{
	vsoutput.tex = tex;
	gl_Position = pos;
}

[fragment]

[include "common.h"]
[include "gbuffer_common.h"]

#define BLUR_SAMPLES 5
[include "poisson_disc_kernel.h"]

[uniform dofparams 1 perframe]
uniform dofparams
{
	float focus;
};

[sampler2D main_texture 3]

in VSOutput vsoutput;

out vec4 color;

#pragma optionNV (unroll all)
vec4 blur(vec2 tex, float depth, float cocvalue)
{
	float weight = 1.0f;
	vec4 color = texture(main_texture, tex);

	vec2 r = cocvalue * 0.5f / textureSize(main_texture, 0);

	for (int i = 0; i < BLUR_SAMPLES; ++i)
	{
		vec2 kernel = disc_kernel[i];
		vec2 offset = kernel * r;
		vec4 c = texture(main_texture, tex + offset);
		float pixel_depth = texture(depth_texture, tex + offset);
		float contribution = abs(pixel_depth - depth) / cocvalue;
		color += c * contribution;
		weight += contribution;
	}

	return vec4(color.rgb / weight, 1.0f);
}
/*
float coc(float focus, float far_blur_distance, float near_blur_distance, float distance)
{
	float f1 = (distance - focus) / (focus - near_blur_distance);
	float f2 = (distance - focus) / (far_blur_distance - focus);
	return saturate(f1 + f2);	
}
*/

float calculate_focal_length(float zfar, float focus)
{
	return 1.0f / (1.0f / focus + 1.0f / zfar);
}

float calculate_coc(float aperture, float focus, float focal_length, float distance)
{
	return abs(aperture * focus * (focus - distance) / (distance * (focus - focal_length)));
}

void main()
{
	float focus_distance = 300.0f;
	float far_dof_blur_distance = 500.0f;
	float near_dof_blur_distance = 100.0f;

	GBuffer gbuffer = gbuffer_unpack(vsoutput.tex);
	float linear_depth = linearized_depth(gbuffer.depth, znear, zfar);

	//float cocvalue = coc(focus, far_dof_blur_distance, near_dof_blur_distance, linear_depth);	
	float coc = calculate_coc(1.0f, focus_distance, calculate_focal_length(zfar, focus), linear_depth);
	vec4 blurred = blur(vsoutput.tex, linear_depth, coc);
	color = vec4(blurred.rgb, 1.0f);
}
