#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float flag;

void main()
{
    FragColor = texture(screenTexture, TexCoords);
	FragColor.y = FragColor.y * (1 - flag);
}