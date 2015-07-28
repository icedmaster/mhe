#define saturate(x) clamp(x, 0.0f, 1.0f)

vec3 position_from_depth(vec2 screenspace_pos, float depth, mat4 inv_vp)
{
	vec4 pos = vec4(screenspace_pos * 2.0f - 1.0f, depth * 2.0f - 1.0f, 1.0f);
	vec4 world_pos = inv_vp * pos;
	return world_pos.xyz / world_pos.w;
}

vec3 mhe_reflect(vec3 I, vec3 N)
{
	return I - 2.0f * dot(I, N) * N;
}

bool check_clamping(vec2 pos)
{
	return pos.x < 0.0f || pos.x > 1.0f || pos.y < 0.0f || pos.y > 1.0f;
}

#define VEC3_ZERO vec3(0.0f, 0.0f, 0.0f)
#define VEC4_ZERO vec4(0.0f, 0.0f, 0.0f, 0.0f);
