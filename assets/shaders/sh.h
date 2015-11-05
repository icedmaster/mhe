struct SH9
{
	float c[9];
};

struct ColorSH9
{
	vec3 c[9];
};

[include "common.h"]

#define COS_A0 PI
#define COS_A1 (2.0f * PI) / 3.0f
#define COS_A2 PI * 0.25f

ColorSH9 fetch_sh9_color(samplerBuffer buffer, int vertex)
{
	int stride = 9 * 3;
	ColorSH9 res;
	for (int i = 0; i < 9; ++i)
	{
		float r = texelFetch(buffer, vertex * stride + i * 3 + 0).r;
		float g = texelFetch(buffer, vertex * stride + i * 3 + 1).r;
		float b = texelFetch(buffer, vertex * stride + i * 3 + 2).r;
		res.c[i] = vec3(r, g, b);
	}

	return res;
}

SH9 sh_cosine_lobe(in vec3 dir)
{
	SH9 sh;

    // Band 0
    sh.c[0] = 0.282095f * COS_A0;

    // Band 1
    sh.c[1] = 0.488603f * dir.y * COS_A1;
    sh.c[2] = 0.488603f * dir.z * COS_A1;
    sh.c[3] = 0.488603f * dir.x * COS_A1;

    // Band 2
    sh.c[4] = 1.092548f * dir.x * dir.y * COS_A2;
    sh.c[5] = 1.092548f * dir.y * dir.z * COS_A2;
    sh.c[6] = 0.315392f * (3.0f * dir.z * dir.z - 1.0f) * COS_A2;
    sh.c[7] = 1.092548f * dir.x * dir.z * COS_A2;
    sh.c[8] = 0.546274f * (dir.x * dir.x - dir.y * dir.y) * COS_A2;

	return sh;
}

vec3 calculate_irradiance(in vec3 nrm, in ColorSH9 radiance)
{
	vec3 res = VEC3_ZERO;
	SH9 cos_lobe = sh_cosine_lobe(nrm);
	for (int i = 0; i < 9; ++i)
		res += cos_lobe.c[i] * radiance.c[i];
	return res / PI;
}

