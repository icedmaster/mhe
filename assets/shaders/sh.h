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
    vec4 r;
    vec4 g;
    vec4 b;
};

[include "common.h"]

#define COS_A0 PI
#define COS_A1 (2.0f * PI) / 3.0f
#define COS_A2 PI * 0.25f

ColorSH9 fetch_sh9_color(samplerBuffer buf, int vertex)
{
	int stride = 9;
	ColorSH9 res;
	for (int i = 0; i < 9; ++i)
	{
		vec3 c = texelFetch(buf, vertex * stride + i).rgb;
		res.c[i] = c;
	}

	return res;
}

ColorSH9 sh9_zero()
{
    ColorSH9 res;
    for (int i = 0; i < 9; ++i)
        res.c[i] = VEC3_ZERO;
    return res;
}

SH9 sh9_cosine_lobe(in vec3 dir)
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

ColorSH9 sh9_project(in vec3 dir, in vec3 color)
{
    SH9 lobe = sh9_cosine_lobe(dir);
    ColorSH9 res;
    for (int i = 0; i < 9; ++i)
        res.c[i] = color * lobe.c[i];
    return res;
}

ColorSH9 add(ColorSH9 sh1, ColorSH9 sh2)
{
    ColorSH9 res;
    for (int i = 0; i < 9; ++i)
        res.c[i] = sh1.c[i] + sh2.c[i];
    return res;
}

ColorSH9 mul(ColorSH9 sh, float f)
{
    ColorSH9 res;
    for (int i = 0; i < 9; ++i)
        res.c[i] = sh.c[i] * f;
    return res;
}

SH4 sh_cosine_lobe_sh4(in vec3 dir)
{
	SH4 sh;

    // Band 0
    sh.c[0] = 0.282095f * COS_A0;

    // Band 1
    sh.c[1] = -0.488603f * dir.y * COS_A1;
    sh.c[2] = 0.488603f * dir.z * COS_A1;
    sh.c[3] = -0.488603f * dir.x * COS_A1;

    return sh;
}

SH4 sh4(in vec3 dir)
{
	SH4 sh;

    // Band 0
    sh.c[0] = 0.282095f;

    // Band 1
    sh.c[1] = -0.488603f * dir.y;
    sh.c[2] = 0.488603f * dir.z;
    sh.c[3] = -0.488603f * dir.x;

    return sh;
}

RGBSH4 mul(SH4 sh, vec3 c)
{
    RGBSH4 res;
    res.r = sh.c * c.x;
    res.g = sh.c * c.y;
    res.b = sh.c * c.z;

    return res;
}

vec3 shdot(SH4 sh, RGBSH4 rgb_sh)
{
    return vec3(dot(sh.c, rgb_sh.r), dot(sh.c, rgb_sh.g), dot(sh.c, rgb_sh.b));
}

RGBSH4 shabs(RGBSH4 sh)
{
    RGBSH4 res;
    res.r = abs(sh.r);
    res.g = abs(sh.g);
    res.b = abs(sh.b);
    return res;
}

RGBSH4 add(RGBSH4 sh1, RGBSH4 sh2)
{
    RGBSH4 res;
    res.r = sh1.r + sh2.r;
    res.g = sh1.g + sh2.g;
    res.b = sh1.b + sh2.b;
    return res;
}

RGBSH4 sub(RGBSH4 sh1, RGBSH4 sh2)
{
    RGBSH4 res;
    res.r = sh1.r - sh2.r;
    res.g = sh1.g - sh2.g;
    res.b = sh1.b - sh2.b;
    return res;
}

vec3 calculate_irradiance(in vec3 nrm, in ColorSH9 radiance)
{
	vec3 res = VEC3_ZERO;
	SH9 cos_lobe = sh9_cosine_lobe(nrm);
	for (int i = 0; i < 9; ++i)
		res += cos_lobe.c[i] * radiance.c[i];
	return res / PI;
}

vec3 calculate_irradiance(in vec3 nrm, in RGBSH4 rgb_sh)
{
    vec3 res = VEC3_ZERO;
    SH4 cos_lobe = sh_cosine_lobe_sh4(nrm);
    res.r = dot(cos_lobe.c, rgb_sh.r);
    res.g = dot(cos_lobe.c, rgb_sh.g);
    res.b = dot(cos_lobe.c, rgb_sh.b);
    return res / PI;
}

RGBSH4 empty_rgbsh4()
{
    RGBSH4 res;
    res.r = VEC4_ZERO;
    res.g = VEC4_ZERO;
    res.b = VEC4_ZERO;
    return res;
}

