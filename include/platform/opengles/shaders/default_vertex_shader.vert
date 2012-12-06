"uniform mat4 projection_matrix;								\n\
																\n\
attribute vec3 position;										\n\
attribute vec4 color;										    \n\
varying lowp vec4 fragment_color;								\n\
																\n\
void main()														\n\
{																\n\
	gl_Position = projection_matrix * vec4(position, 1.0);		\n\
	fragment_color = color;										\n\
}																\n\
"
