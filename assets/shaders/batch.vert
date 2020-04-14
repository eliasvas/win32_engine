#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coords;
layout(location = 2) in vec2 wps;
layout(location = 3) in vec2 scale;
layout(location = 4) in uint unit;

out float red;
out vec2 f_tex_coords;
flat out uint tex_unit;

void main()
{
	gl_Position = vec4(position.x + wps.x, position.y + wps.y, 0.0, 1.0);
	tex_unit = unit;
	f_tex_coords = tex_coords;
}