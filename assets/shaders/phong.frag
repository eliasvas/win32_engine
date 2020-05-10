#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
out vec4 FragColor;
  
in vec3 f_normal;
in vec3 w_frag_pos;

uniform vec3 view_pos; //camera position
uniform Material m;
uniform Light light;


void main()
{
	vec3 ambient = m.ambient;
	ambient = light.diffuse * ambient;
	
	vec3 norm = normalize(f_normal);
	vec3 light_dir = normalize(light.position - w_frag_pos);
	
	float diff = max(dot(norm,light_dir),0.0);
	vec3 diffuse = diff * m.diffuse;
	diffuse = light.diffuse * diffuse;
	
	vec3 view_dir = normalize(light.position - w_frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 12);
	
	vec3 specular = m.shininess * spec * m.specular;
	specular = light.specular * specular;
	
    FragColor = vec4((diffuse + ambient + specular), 1.0);
} 

