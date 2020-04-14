#version 330 core

out vec4 FragColor;

flat in uint tex_unit;

in vec2 f_tex_coords;


uniform sampler2D slots[2];
uniform sampler2D slot;

void main(){
	//float r = float(sslots[tex_unit]);                      //its 2^(30) which is the max of int(32) for some reason
	if (tex_unit != 1 && tex_unit!= 0)FragColor = vec4(float(tex_unit)/1140000000.0,0.0,0.0,1.0);
	else
		FragColor = texture(slots[tex_unit], f_tex_coords);
}  