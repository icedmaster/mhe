#define saturate(x) clamp(x, 0.0f, 1.0f)

vec3 position_from_depth(vec2 screenspace_pos, float depth, mat4 inv_vp)
{
	vec4 pos = vec4(screenspace_pos * 2.0f - 1.0f, depth * 2.0f - 1.0f, 1.0f);
	vec4 world_pos = inv_vp * pos;
	return world_pos.xyz / world_pos.w;
}

vec3 position_from_linear_depth(vec2 screenspace_pos, float depth, float tanfov, float aspect_ratio)
{
	return vec3(0.0f, 0.0f, 0.0f);
}

vec2 position_to_screen_position(vec3 pos, mat4 m)
{
	vec4 pos_cs = m * vec4(pos, 1.0f);
	return (pos_cs.xy / pos_cs.w) * 0.5f + 0.5f;
}

vec3 mhe_reflect(vec3 I, vec3 N)
{
	return I - 2.0f * dot(I, N) * N;
}

bool check_clamping(vec2 pos)
{
	return pos.x < 0.0f || pos.x > 1.0f || pos.y < 0.0f || pos.y > 1.0f;
}

float random(vec2 pos, float seed)
{
	float t = seed * dot(pos, vec2(12.9898, 78.233));
	return fract( sin(t) * 43758.5453 );
}

float max3(vec3 v)
{
    return max(v.x, max(v.y, v.z));
}


#define VEC2_ZERO vec2(0.0f, 0.0f)
#define VEC3_ZERO vec3(0.0f, 0.0f, 0.0f)
#define VEC4_ZERO vec4(0.0f, 0.0f, 0.0f, 0.0f);

#define PI 3.14159265359f
