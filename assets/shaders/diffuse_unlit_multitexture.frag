#version 330 core											
in vec4 fragment_color;			
in vec2 fragment_texcoord0;
in vec2 fragment_texcoord1;
in vec2 fragment_texcoord2;		
											
uniform sampler2D texture_unit0;				
uniform sampler2D texture_unit1;				
uniform sampler2D texture_unit2;				
											
out vec4 color;								
											
void main()									
{											
	gl_FragColor = fragment_color * (texture2D(texture_unit0, fragment_texcoord0) +
									 texture2D(texture_unit1, fragment_texcoord1) + texture2D(texture_unit2, fragment_texcoord2));
}											

