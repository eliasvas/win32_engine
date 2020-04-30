#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float flag;

void main()
{
    FragColor = vec4(vec3(1 -texture(screenTexture, TexCoords)) * flag +vec3(texture(screenTexture, TexCoords)) * (1 - flag) , 1.0);
}