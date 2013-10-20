#version 330 core											
uniform mat4 projection_matrix;								
																
in vec3 position;										
in vec4 color;										    
in vec2 texcoord0;	
in vec2 texcoord1;
in vec2 texcoord2;									
out vec4 fragment_color;								
out vec2 fragment_texcoord0;
out vec2 fragment_texcoord1;
out vec2 fragment_texcoord2;							
																
void main()														
{																
	gl_Position = projection_matrix * vec4(position, 1.0);		
	fragment_color = color;										
	fragment_texcoord0 = texcoord0;								
	fragment_texcoord1 = texcoord1;
	fragment_texcoord2 = texcoord2;
}																

