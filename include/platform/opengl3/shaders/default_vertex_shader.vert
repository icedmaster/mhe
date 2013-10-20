"#version 330 core											\n\
uniform mat4 projection_matrix;								\n\
																\n\
in vec3 position;										\n\
in vec4 color;										    \n\
in vec2 texcoord0;										\n\
out vec4 fragment_color;								\n\
out vec2 fragment_texcoord;							\n\
																\n\
void main()														\n\
{																\n\
	gl_Position = projection_matrix * vec4(position, 1.0);		\n\
	fragment_color = color;										\n\
	fragment_texcoord = texcoord0;								\n\
}																\n\
"
