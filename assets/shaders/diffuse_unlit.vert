#version 330 core											
uniform mat4 projection_matrix;								
																
in vec3 position;										
in vec4 color;										    
in vec2 texcoord0;										
out vec4 fragment_color;								
out vec2 fragment_texcoord;							
																
void main()														
{																
	gl_Position = projection_matrix * vec4(position, 1.0);		
	fragment_color = color;										
	fragment_texcoord = texcoord0;								
}																

