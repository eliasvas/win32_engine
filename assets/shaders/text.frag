#version 330 core
in vec2 UV;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform float time;

void main(){

	color = texture( myTextureSampler, UV );
	//color = vec4(1,1,1,1);
	
}