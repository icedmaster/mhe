[samplerBuffer skin_texture 1]

mat4 read_matrix(samplerBuffer t, int index)
{
	mat4 m = mat4(texelFetch(t, index * 4 + 0),
		   	 	  texelFetch(t, index * 4 + 1),
				  texelFetch(t, index * 4 + 2),
				  texelFetch(t, index * 4 + 3));
	return m;				   
}

void skinning(out mat4 transform, samplerBuffer skin_texture, ivec4 ids, vec4 weights)
{
	mat4 m1 = read_matrix(skin_texture, ids[0]);
	mat4 m2 = read_matrix(skin_texture, ids[1]);
	mat4 m3 = read_matrix(skin_texture, ids[2]);
	mat4 m4 = read_matrix(skin_texture, ids[3]);

	transform = m1 * weights[0] + m2 * weights[1] + m3 * weights[2] + m4 * weights[3];
}
