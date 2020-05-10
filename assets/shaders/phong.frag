#version 330 core
out vec4 FragColor;
  
  
in vec3 f_normal;
in vec3 w_frag_pos;

uniform vec3 light_pos;
uniform vec3 view_pos; //camera position
float specular_str = 0.4;

void main()
{
	vec3 ambient = vec3(0.2,0.2,0.2);
	
	vec3 norm = normalize(f_normal);
	vec3 light_dir = normalize(light_pos - w_frag_pos);
	
	float diff = max(dot(norm,light_dir),0.0);
	vec3 diffuse = diff * vec3(0.5,0.2,0.2);
	
	vec3 view_dir = normalize(light_pos - w_frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 128);
	vec3 specular = specular_str * spec * vec3(0.1,0.1,0.1);
	
    FragColor = vec4((diffuse + ambient + spec), 1.0);
} 
