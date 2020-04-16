#version 330 core

out vec4 FragColor;

flat in uint tex_unit;

in vec2 f_tex_coords;
in float f_opacity;

uniform sampler2D slots[32];
uniform vec2 tex_sizes[32];

void main(){
	FragColor = texture(slots[tex_unit], f_tex_coords);
	if (FragColor.a < 0.01)
	{
		discard;
	}else
	{
		FragColor.a = f_opacity;
	}

}  