"#version 330 core											\n\
in vec4 fragment_color;			\n\
in vec2 fragment_texcoord;		\n\
											\n\
uniform sampler2D texture_unit;				\n\
											\n\
out vec4 color;								\n\
											\n\
void main()									\n\
{											\n\
	gl_FragColor = fragment_color * texture2D(texture_unit, fragment_texcoord);	\n\
}											\n\
"
