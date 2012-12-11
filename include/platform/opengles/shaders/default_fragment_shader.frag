"varying lowp vec4 fragment_color;			\n\
varying highp vec2 fragment_texcoord;		\n\
											\n\
uniform sampler2D texture_unit;				\n\
											\n\
void main()									\n\
{											\n\
	gl_FragColor = fragment_color * texture2D(texture_unit, fragment_texcoord);	\n\
}											\n\
"
