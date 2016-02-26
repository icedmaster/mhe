struct SH9
{
	float c[9];
};

struct ColorSH9
{
	vec3 c[9];
};

struct SH4
{
    vec4 c;
};

struct ColorSH4
{
    vec3 c[4];
};

struct RGBSH4
{
    vec4 rgb[3];
};

[include "common.h"]

#define COS_A0 PI
#define COS_A1 (2.0f * PI) / 3.0f
#define COS_A2 PI * 0.25f

ColorSH9 fetch_sh9_color(samplerBuffer buffer, int vertex)
{
	int stride = 9;
	ColorSH9 res;
	for (int i = 0; i < 9; ++i)
	{
		vec3 c = texelFetch(buffer, vertex * stride + i).rgb;
		res.c[i] = c;
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

SH4 sh_cosine_lobe_sh4(in vec3 dir)
{
	SH4 sh;

    // Band 0
    sh.c[0] = 0.282095f * COS_A0;

    // Band 1
    sh.c[1] = 0.488603f * dir.y * COS_A1;
    sh.c[2] = 0.488603f * dir.z * COS_A1;
    sh.c[3] = 0.488603f * dir.x * COS_A1;

    return sh;
}

RGBSH4 mul(SH4 sh, vec3 c)
{
    RGBSH4 res;
    res.rgb[0] = sh.c * c.x;
    res.rgb[1] = sh.c * c.y;
    res.rgb[2] = sh.c * c.z;

    return res;
}

RGBSH4 add(RGBSH4 sh1, RGBSH4 sh2)
{
    RGBSH4 res;
    res.rgb[0] = sh1.rgb[0] + sh2.rgb[0];
    res.rgb[1] = sh1.rgb[1] + sh2.rgb[1];
    res.rgb[2] = sh1.rgb[2] + sh2.rgb[2];
    return res;
}

vec3 calculate_irradiance(in vec3 nrm, in ColorSH9 radiance)
{
	vec3 res = VEC3_ZERO;
	SH9 cos_lobe = sh_cosine_lobe(nrm);
	for (int i = 0; i < 9; ++i)
		res += cos_lobe.c[i] * radiance.c[i];
	return res / PI;
}

vec3 calculate_irradiance(in vec3 nrm, in RGBSH4 rgb_sh)
{
    vec3 res = VEC3_ZERO;
    SH4 cos_lobe = sh_cosine_lobe_sh4(nrm);
    res.r = dot(cos_lobe.c, rgb_sh.rgb[0]);
    res.g = dot(cos_lobe.c, rgb_sh.rgb[1]);
    res.b = dot(cos_lobe.c, rgb_sh.rgb[2]);
    return res / PI;
}

RGBSH4 empty_rgbsh4()
{
    RGBSH4 res;
    res.rgb[0] = VEC4_ZERO;
    res.rgb[1] = VEC4_ZERO;
    res.rgb[2] = VEC4_ZERO;
    return res;
}

