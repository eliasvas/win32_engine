#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coords;
layout(location = 2) in vec2 wps;
layout(location = 3) in vec2 scale;
layout(location = 4) in uint unit;
layout(location = 5) in float opacity;

out float f_opacity;
out vec2 f_tex_coords;
flat out uint tex_unit;
uniform float iTime;

uniform mat4 projection_matrix;

void main()
{
	vec2 real_position = position;
	//gl_Position = vec4(real_position.x + wps.x, real_position.y + wps.y, cos(iTime) * (-1800.0), 1.0);
	gl_Position = vec4(real_position.x + wps.x, real_position.y + wps.y,0.0, 1.0);
	gl_Position = projection_matrix * gl_Position;
	tex_unit = unit;
	f_tex_coords = tex_coords;
	f_opacity = opacity;
}