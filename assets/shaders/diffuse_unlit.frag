#version 330 core											
in vec4 fragment_color;			
in vec2 fragment_texcoord;		
											
uniform sampler2D texture_unit0;				
											
out vec4 color;								
											
void main()									
{											
	gl_FragColor = fragment_color * texture2D(texture_unit0, fragment_texcoord);	
}											

