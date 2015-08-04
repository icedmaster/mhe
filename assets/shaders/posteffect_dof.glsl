// Resources:
// http://amd-dev.wpengine.netdna-cdn.com/wordpress/media/2012/10/Scheuermann_DepthOfField.pdf
// https://mynameismjp.wordpress.com/2011/02/28/bokeh/

[defs PASS 0 2]

#define BLUR_RESOLVE_PASS 0
#define DOF_PASS 1
#define COMPOSITE_PASS 2

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

[include "poisson_disc_kernel.h"]

[uniform dofparams 1 perframe]
uniform dofparams
{
	vec4 planes; // x - focus plane, y - near blur plane, z - far blur plane, w - max blurriness
	vec4 coc; // x - diameter
};

[sampler2D main_texture 3]
[sampler2D blur_texture 4]
[sampler2D dof_texture 5]

in VSOutput vsoutput;

#if PASS == BLUR_RESOLVE_PASS
out float color;
#else
out vec4 color;
#endif

#pragma optionNV (unroll all)
vec4 blur(vec2 tex, vec4 pixel_color, float depth, float blur)
{
	float weight = 1.0f;
	vec4 color = pixel_color;

	ivec2 texsize = textureSize(main_texture, 0);

	vec2 radius = blur * coc.x / texsize;

	for (int i = 0; i < DISC_SAMPLES / 2; ++i)
	{
		vec2 kernel = disc_kernel[i];
		vec2 offset = kernel * radius;
		vec4 c = texture(main_texture, tex + offset);
		float pixel_depth = texture(depth_texture, tex + offset);
		float pixel_blurriness = texture(blur_texture, tex + offset);
		float contribution = pixel_blurriness;
		color += c * contribution;
		weight += contribution;
	}

	return vec4(color.rgb / weight, 1.0f);
}

float calculate_blur(float focus, float far_blur_distance, float near_blur_distance, float distance, float max_blurriness)
{
	float f;
	if (distance < focus)
	    f = (focus - distance) / (focus - near_blur_distance);
	else
		f = (distance - focus) / (far_blur_distance - focus);
	return clamp(f, 0.0f, max_blurriness);
}

float calculate_focal_length(float zfar, float focus)
{
	return 1.0f / (1.0f / focus + 1.0f / zfar);
}

float calculate_coc(float aperture, float focus, float focal_length, float distance)
{
	return abs(aperture * focus * (focus - distance) / (distance * (focus - focal_length)));
}

#if PASS == BLUR_RESOLVE_PASS
void main()
{
	float focus_distance = planes.x;
	float near_dof_blur_distance = planes.y;
	float far_dof_blur_distance = planes.z;
	float max_blurriness = planes.w;

	GBuffer gbuffer = gbuffer_unpack(vsoutput.tex);
	float linear_depth = linearized_depth(gbuffer.depth, znear, zfar);

	vec4 pixel_color = texture(main_texture, vsoutput.tex);

	float blurriness = calculate_blur(focus_distance, far_dof_blur_distance, near_dof_blur_distance, linear_depth, max_blurriness);	
	color = blurriness;
}
#elif PASS == DOF_PASS
void main()
{
	GBuffer gbuffer = gbuffer_unpack(vsoutput.tex);
	float linear_depth = linearized_depth(gbuffer.depth, znear, zfar);

	vec4 pixel_color = texture(main_texture, vsoutput.tex);
	float blurriness = texture(blur_texture, vsoutput.tex).r;

	vec4 blurred = blur(vsoutput.tex, pixel_color, linear_depth, blurriness);
	color = blurred;
}
#elif PASS == COMPOSITE_PASS
void main()
{
	float blur = texture(blur_texture, vsoutput.tex).r;
	vec4 dof_value = texture(dof_texture, vsoutput.tex);
	vec4 original = texture(main_texture, vsoutput.tex);
	
	color = mix(original, dof_value, saturate(blur));
}
#endif
